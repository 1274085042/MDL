#pragma once
#include "../facilities/traits.h"
#include "../facilities/policies.h"
#include "../facilities/common_io.h"
#include "../../policies/policy_selector.h"
#include "../../policies/policy_container.h"
#include "../../policies/policy_operations.h"
#include "../../model_rel/param_initializer/facilities/traits.h"

#include <string>
#include <ostream>

namespace MDL
{
    template <typename tPolicyContainer>
    class BiasLayer
    {
        static_assert(IsPolicyContainer<tPolicyContainer>, "Parameter is not policy container.");
        using CurPolicyContainer = PlainPolicy<tPolicyContainer>;

    public:
        static constexpr bool IsFeedbackOutput = PolicySelect<FeedbackPolicy, CurPolicyContainer>::IsFeedbackOutput;
        static constexpr bool IsUpdate = PolicySelect<FeedbackPolicy, CurPolicyContainer>::IsUpdate;
        using InputType = LayerIO;
        using OutputType = LayerIO;

    private:
        using ElementType = typename PolicySelect<OperandPolicy, CurPolicyContainer>::Element;
        using DeviceType = typename PolicySelect<OperandPolicy, CurPolicyContainer>::Device;

        const std::string name_;
        size_t rowNum_;
        size_t colNum_;
        
        Matrix<ElementType, DeviceType> bias_;

        // DataType 的形式为 stack<DynamicData<ElementType, DeviceType, CategoryTags>>
        using DataType = LayerTraits::LayerInternalBuf<IsUpdate, PolicySelect<InputPolicy, CurPolicyContainer>::BatchMode, ElementType, DeviceType, CategoryTags::Matrix, CategoryTags::MatrixBatch>;
        DataType grad_;
    
    public:
        BiasLayer(std::string name, size_t vecLen):name_(name)
        {
            if(vecLen == 0)
            {
                throw std::runtime_error("Invalidate input len for bias layer");
            }

            rowNum_ = 1;
            colNum_ = vecLen;
        }

        BiasLayer(std::string name, size_t rowNum, size_t colNum):name_(name), rowNum_(rowNum), colNum_(colNum)
        {
            if((rowNum_==0) || (colNum_==0))
            {
                throw std::runtime_error("Invalidate row/col num for bias layer.");
            }
        }

        template<typename tInitializer, typename tBuffer, typename tInitPolicyContainer = typename tInitializer::PolicyCont>
        void Init(tInitializer &initializer, tBuffer &loadBuffer, std::ostream *log = nullptr)
        {
            /*
            * 1 当前层和网络中的其它层共享参数矩阵
            * 如果loadBuffer中存在某个矩阵，其名称和正要初始化的矩阵名字相同，
            * 说明当前层和网络中的其它层共享参数矩阵
            */
            if (auto it = loadBuffer.find(name_); it != loadBuffer.end())
            {
                const Matrix<ElementType, DeviceType> &m = it->second;
                if((m.RowNum() != rowNum_) || (m.ColNum() != colNum_))
                {
                    throw std::runtime_error("Load matrix error in BiasLayer");
                }
                bias_ = m;
                if(log)
                {
                    std::string logInfo = "Load from loadBuffer: " + name_ + '\n';
                    (*log)<<logInfo;
                }

                return;
            }

            /*
            * 2 加载预训练权重
            * 如果initializer存在某个矩阵和当前层的矩阵名称一样，将initializer中的矩阵拷贝到当前矩阵
            */
            else if(initializer.IsMatrixExist(name_))
            {
                bias_ = Matrix<ElementType, DeviceType> (rowNum_, colNum_);
                initializer.GetMatrix(name_, bias_);
                loadBuffer[name_] = bias_;
                if(log)
                {
                    std::string logInfo = "Copy from initializer: " + name_ + '\n';
                    (*log) << logInfo; 
                }
                return;
            }

            /*
            * 3 使用初始化器初始化参数
            */
            else
            {
                bias_ = Matrix<ElementType, DeviceType> (rowNum_, colNum_);
                using CurInitializer = PickInitializer<tInitPolicyContainer, InitPolicy::BiasTypeCate>;

                if constexpr(!std::is_same<CurInitializer, void>::value)
                {
                    size_t size = rowNum_ * colNum_;
                    auto &curInit = initializer.template GetFiller<CurInitializer>();
                    curInit.Fill(bias_);
                    loadBuffer[name_] = bias_;
                    if(log)
                    {
                        std::string logInfo = "Random init from initializer " + name_ + '\n';
                        (*log)<<logInfo;
                    }
                }
                else
                {
                    throw std::runtime_error("Cannot get initializer for InitPolicy::BiasTypeCate");
                }
            }
        }

        template <typename T>
        void SaveWeights(T &saver)
        {
            auto it = saver.find(name_);
            if( (it != saver.end()) && (it->second != bias_))
            {
                throw std::runtime_error("Duplicate save for bias " +  name_);
            }

            saver[name_] = bias_;
        }

        template <typename T>
        auto FeedForward(const T &in)
        {
            const auto & val = in.template Get<LayerIO>();
            return OutputType::Create().template Set<LayerIO>(val + bias_);
        }
        
        template <typename tGrad>
        auto FeedBackward(const tGrad & grad)
        {

            if constexpr(IsUpdate)
            {
                const auto & tmp = grad.template Get<LayerIO> ();
                assert((tmp.RowNum() == bias_.RowNum()) && (tmp.ColNum() == bias_.ColNum()));
                grad_.push(MakeDynamicData(tmp));
            }

            if constexpr(IsFeedbackOutput)
            {
                return grad;
            }
            else
            {
                return OutputType::Create();
            }
        }

        template <typename tGradCollector>
        void GradCollect(tGradCollector &coll)
        {
            if constexpr (IsUpdate)
            {
                LayerTraits::MatrixGradCollect(bias_, grad_ ,coll);
            }
        }
        
        void NeutralInvariant()
        {
            if constexpr(IsUpdate)
            {
                if(!(grad_.empty()))
                {
                    throw std::runtime_error("NeutralInvariant fail!");
                }
            }
        }
    };
}
