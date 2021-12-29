#pragma once

#include "./eval_unit.h"
#include <list>
#include <memory>

namespace MDL
{
    /*
    * EvalGroup用于整合相同的EvalUnit，合并同类的计算，提升求值效率
    * 派生类需实现Merge接口，用于添加EvalUnit
    * 
    * 在许多深度学习框架中，合并同类计算是由第三库完成的，
    * 如Intel的MathKernelLibrary(MKL)、CUDA它们提供了批处理接口，
    * 一次性完成多个矩阵的运算
    */
    template<typename tDevice>
    class BaseEvalGroup
    {
    public:
        virtual std::shared_ptr<BaseEvalUnit<tDevice>> GetEvalUnit() = 0;
        virtual void Merge(BaseEvalUnit<tDevice> &) = 0;
        virtual void Merge(BaseEvalUnit<tDevice> &&) = 0;
        virtual ~BaseEvalGroup() = default;
    };

    /*
    * 平凡的TrivialEvalGroup
    * 不会将多个EvalUnit进行合并，只是简单的积累EvalUnit
    */
    template<typename tEvalUnit>
    class TrivialEvalGroup: public BaseEvalGroup<typename tEvalUnit::DeviceType>
    {
    private:
        using DeviceType = typename tEvalUnit::DeviceType;
        std::list<tEvalUnit> unitList_;

    public:
        void Merge(BaseEvalUnit<DeviceType> &unit)
        {
            unitList_.push_back(static_cast<tEvalUnit &> (unit));
        }
        void Merge(BaseEvalUnit<DeviceType> &&unit)
        {
            unitList_.push_back(static_cast<tEvalUnit &&> (unit));
        }

        std::shared_ptr<BaseEvalUnit<DeviceType>> GetEvalUnit()
        {
            std::shared_ptr<BaseEvalUnit<DeviceType>> res = nullptr;
            if(!unitList_.empty())
            {
                res = std::make_shared<tEvalUnit>(std::move(unitList_.front()));
                unitList_.pop_front();
            }

            return res;
        }

    };
}