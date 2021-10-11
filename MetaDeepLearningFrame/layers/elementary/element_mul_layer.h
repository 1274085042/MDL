#pragma once
#include "../facilities/traits.h"
#include "../facilities/policies.h"
#include "../facilities/common_io.h"
#include "../../policies/policy_selector.h"
#include "../../policies/policy_container.h"
#include "../../policies/policy_operations.h"

namespace MDL
{
    using ElementMulLayerInput = VarTypeDict<struct ElementMulLayerIn1, struct ElementMulLayerIn2>;

    template <typename tPolicyContainer>
    class ElementMulLayer
    {
        static_assert(IsPolicyContainer<tPolicyContainer>, "tPolicyContainer is not a policy container.");
        using CurPolicyContainer = PlainPolicy<tPolicyContainer>;

    public:
        static constexpr bool IsFeedbackOutput = PolicySelect<FeedbackPolicy, CurPolicyContainer>::IsFeedbackOutput;
        static constexpr bool IsUpdate = false;
        using InputType = ElementMulLayerInput;
        using OutputType = LayerIO;

    private:
        /*
        * 如果需要反向传播
        * DataType = stack<Dynamic<ElementType, DeviceType, CateType>>
        */
        using DataType = LayerTraits::LayerInternalBuf<IsFeedbackOutput, PolicySelect<InputPolicy, CurPolicyContainer>::BatchMode, typename PolicySelect<OperandPolicy, CurPolicyContainer>::Element, typename PolicySelect<OperandPolicy, CurPolicyContainer>::Device, CategoryTags::Matrix, CategoryTags::MatrixBatch>;
        DataType data1_;
        DataType data2_; 

    public:
        template <typename tIn>
        auto FeedForward(const tIn &in)
        {
            const auto & val1 = in.template Get<ElementMulLayerIn1> ();
            const auto & val2 = in.template Get<ElementMulLayerIn2> ();

            using RawType1 = std::decay_t<decltype(val1)>;
            using RawType2 = std::decay_t<decltype(val2)>;

            static_assert(!std::is_same<RawType1, NullParameter>::value, "parameter1 is invalid");
            static_assert(!std::is_same<RawType2, NullParameter>::value, "parameter2 is invalid");

            if constexpr(IsFeedbackOutput)
            {
                data1_.push(MakeDynamicData(val1));
                data2_.push(MakeDynamicData(val2));
            }

            return OutputType::Create().template Set<LayerIO>(val1*val2);
        }

        template<typename tGrad>
        auto FeedBackward(const tGrad & grad)
        {
            if constexpr(IsFeedbackOutput)
            {
                if((data1_.empty()) || (data2_.empty()))
                {
                    throw std::runtime_error("Cannot do FeedBackward for ElementMulLayer.");
                }

                auto top1 = data1_.top();
                auto top2 = data2_.top();
                data1_.pop();
                data2_.pop();

                auto gradEval = grad.template Get<LayerIO> ();
                return ElementMulLayerInput::Create().template Set<ElementMulLayerIn1>(gradEval * top2).template Set<ElementMulLayerIn2> (gradEval * top1);
            }
            else
            {
                return ElementMulLayerInput::Create();
            }
        }

        void NeutralInvariant()
        {
            if constexpr(IsFeedbackOutput)
            {
                if((!data1_.empty()) || (!data2_.empty()))
                {
                    throw std::runtime_error("NeutralInvariant Fail!");
                }
            }
        }
    };
}