#pragma once
#include "../facilities/policies.h"
#include "../facilities/common_io.h"
#include "../../facilities/null_param.h"
#include "../../policies/policy_selector.h"
#include "../../policies/policy_container.h"
#include "../../policies/policy_operations.h"

namespace MDL
{
    using AddLyaerInput = VarTypeDict<struct AddLayerIn1, struct AddLayerIn2>;

    template <typename tPolicyContainer>
    class AddLayer
    {
        static_assert(IsPolicyContainer<tPolicyContainer>, "tPolicyContainer is not a PolicyContainer");
        using CurPolicyContainer = PlainPolicy<tPolicyContainer>;     // 过滤掉SubPolicyContainer
        using PolicyRes = PolicySelect<FeedbackPolicy, CurPolicyContainer>;

    public:
        static constexpr bool IsFeedbackOutput = PolicyRes::IsFeedbackOutput;
        static constexpr bool IsUpdate = false;

        using InputType = AddLyaerInput;
        using OutputType = LayerIO;

        template <typename tIn>
        auto FeedForward(tIn &in)
        {
            const auto & val1 = in.template Get<AddLayerIn1>();
            const auto & val2 = in.template Get<AddLayerIn2>();
            
            using RawType1 = std::decay_t<decltype(val1)>;
            using RawType2 = std::decay_t<decltype(val2)>;

            static_assert(!std::is_same<RawType1, NullParameter>::value, "Parameter is invalid");
            static_assert(!std::is_same<RawType2, NullParameter>::value, "Parameter is invalid");

            return OutputType::Create().template Set<LayerIO>(val1+val2);
        }

        template <typename tGrad>
        auto FeedBackWard(tGrad && grad)
        {
            if constexpr (IsFeedbackOutput)
            {
                auto res = grad.template Get<LayerIO>();
                return AddLyaerInput::Create().template Set<AddLayerIn1>(res).template Set<AddLayerIn2>(res);
            }
            else
            {
                return AddLyaerInput::Create();
            }
        }

    };
}