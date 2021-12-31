#pragma once

#include <memory>
#include <vector>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <typeindex>
#include <unordered_set>
#include <unordered_map>

#include "./eval_group.h"
#include "./eval_pool.h"
#include "../cpu/trivial_eval_pool.h"

namespace MDL
{

    inline size_t OperandDepth(const std::unordered_map<const void *, size_t> &depMap, const std::vector<const void *> &paramPtr)
    {
        int res = -1;

        for(auto p : paramPtr)
        {
            auto it = depMap.find(p);
            if(it != depMap.end())
            {
                res=std::max(res, (int)(it->second));
            }
        }

        return (size_t)res;
    }

    //EvalCluster保存求值计算请求
    //type_index用以区分不同的EvalGroup类型
    template <typename tDevice>
    using EvalCluster = std::unordered_map<std::type_index, std::shared_ptr<BaseEvalGroup<tDevice>>>;

    template <typename tDevice>
    class EvalLayer
    {
    private:
        //evalSeq_刻画求值过程中的顺序性
        std::vector<EvalCluster<tDevice>> evalSeq_;
        std::unordered_set<const void *> operands_;
        std::unordered_map<const void *, size_t> outputs_;

    public:
        size_t Size()
        {
            return evalSeq_.size();
        }

        EvalCluster<tDevice> &operator[](size_t i)
        {
            return evalSeq_[i];
        }

        bool Empty()
        {
            return evalSeq_.empty();
        }

        void Clear()
        {
            evalSeq_.clear();
            operands_.clear();
            outputs_.clear();
        }

        template <typename tEvalGroup, typename tEvalUnit>
        void EvalRegister(tEvalUnit &&evalReq, const void *resPtr, const std::vector<const void *> &paramPtr)
        {
            if(!resPtr)
            {
                return;
            }

            if(outputs_.find(resPtr) != outputs_.end())
            {
                std::cout<< "Duplicate register." << std::endl;
                return;
            }

            size_t depth = OperandDepth(outputs_, paramPtr) + 1;

            if(evalSeq_.size() <= depth)
            {
                evalSeq_.resize(depth + 1);
            }

            EvalCluster<tDevice> &ec = evalSeq_[depth];

            const auto typeIndex = std::type_index(typeid(tEvalGroup));
            auto it = ec.find(typeIndex);

            if(it == ec.end())
            {
                it = ec.insert({typeIndex, std::make_shared<tEvalGroup>()}).first; //first，插入元素的迭代器
            }

            it->second->Merge(std::forward<tEvalUnit> (evalReq));

            outputs_.insert({resPtr, depth});
        }

    };

    /*
    * EvalPlan接受求值请求，组织求值过程并提供Eval接口触发求值计算
    */
    template <typename tDevice>
    class EvalPlan
    {
    private:
        std::list<EvalLayer<tDevice>> evalLayers_;
        BaseEvalPool<tDevice> *evalPool_;

        static EvalPoolEnum& GlobalEvalPool()
        {
            static EvalPoolEnum inst = EvalPoolEnum::Trival;
            return inst;
        }

        static EvalPoolEnum &ThreadEvalPool()
        {
            static thread_local EvalPoolEnum inst = GlobalEvalPool();  //thread_locl修饰的对象在各个线程中是相互独立的
            return inst;
        }

        static EvalPlan& ThreadInst()
        {
            static thread_local EvalPlan inst;
            return inst;
        }

        EvalPlan():evalPool_(nullptr)
        {
            evalLayers_.resize(1);
        }

        template<typename tEvalGroup, typename tEvalUnit>
        void EvalRegister(tEvalUnit &&evalReq, const void *outputPtr, const std::vector<const void *> &paramPtr)
        {
            //list中的back操作返回的是元素的引用
            auto &curLayer = evalLayers_.back();
            curLayer.template EvalRegister<tEvalGroup>(std::forward<tEvalUnit>(evalReq), outputPtr, paramPtr);
        }

        /*
        * DoLayerEval()处理当前层的求值请求
        */
        void DoLayerEval()
        {
            EvalLayer<tDevice> &curLayer = evalLayers_.back();
            if(curLayer.Empty())
            {
                return;
            }

            evalLayers_.push_back(EvalLayer<tDevice>{});

            size_t seqLen = curLayer.Size();

            for (size_t i = 0; i < seqLen; ++i)    //遍历vector
            {
                EvalCluster<tDevice> & ec = curLayer[i];
                for(auto &e: ec)                   //遍历map，map里面存放了不同种类的计算
                {
                    //处理同类所有计算
                    while(auto unit = e.second->GetEvalUnit())
                    {
                        evalPool_->Process(unit);
                    }
                }

                evalPool_->Barrier();
                //处理当前计算次序时，有没有引入新的计算请求
                if(!evalLayers_.back().Empty())
                {
                    DoLayerEval();
                }
            }

            evalLayers_.pop_back();
            curLayer.Clear();
        }   

    public:
        static void SetEvalPool(EvalPoolEnum ep)
        {
            GlobalEvalPool() = ep;
        }

        /*
        * evalReq：实际的计算单元
        * ouputPtr：指向计算结果的指针
        * paramPtr：指向计算参数的指针
        */
        template<typename tEvalGroup, typename tEvalUnit>
        static void Register(tEvalUnit &&evalReq, const void * ouputPtr, const std::vector<const void *> &paramPtr)
        {
            ThreadInst().template EvalRegister<tEvalGroup>(std::forward<tEvalUnit>(evalReq), ouputPtr, paramPtr);
        }

        /*
        * Eval()处理当前层（EvalLayer）的求值请求
        */
        static void Eval()
        {
            EvalPlan& plan = ThreadInst();

            if((ThreadEvalPool() != GlobalEvalPool()) || (!plan.evalPool_))
            {
                switch(GlobalEvalPool())
                {
                case EvalPoolEnum::Trival:
                    plan.evalPool_ = &(TrivialEvalPool<tDevice>::Instance());
                    break;

                default:
                    assert(false);
                }

                ThreadEvalPool() = GlobalEvalPool();
            }

            if(!plan.evalPool_)
            {
                throw std::runtime_error("No evaluation pool is available!");
            }

            plan.DoLayerEval();
        }
    };

    template <typename tData>
    auto Evaluate(const tData &data)
    {
        using DeviceType = typename tData::DeviceType;
        auto evalHandle = data.EvalRegister();
        EvalPlan<DeviceType>::Eval();
        return evalHandle.Data();
    }
}
