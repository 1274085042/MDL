#pragma once

#include <random>
#include "../../../policies/policy_macro_begin.h"

namespace MDL
{
    struct InitPolicy
    {
        using MajorClass = InitPolicy;

        struct OverallTypeCate;
        using Overall = void;

        struct WeightTypeCate;
        using Weight = void;

        struct BiasTypeCate;
        using Bias = void;

        struct RandEngineTypeCate;
        using RandEngine = std::mt19937;
    };

    TypePolicyTemplate(PInitializerIs, InitPolicy, Overall);
    TypePolicyTemplate(PWeightInitializerIs, InitPolicy, Weight);
    TypePolicyTemplate(PBiasInitializerIs, InitPolicy, Bias);
    TypePolicyTemplate(PRandEngineInitializerIs, InitPolicy, RandEngine);

    struct VarScaleFillerPolicy
    {
        using MajorClass = VarScaleFillerPolicy;

        struct DistributeTypeCate
        {
            struct Uniform;
            struct Norm;
        };
        using Distribute = DistributeTypeCate::Uniform;

        struct ScaleModeTypeCate
        {
            struct FanIn;
            struct FanOut;
            struct FanAvg;
        };
        using ScaleMode = ScaleModeTypeCate::FanAvg;
    };

    TypePolicyObj(PNormVarScale, VarScaleFillerPolicy, Distribute, Norm);
    TypePolicyObj(PUniformVarScale, VarScaleFillerPolicy, Distribute, Uniform);
    TypePolicyObj(PFanInVarScale, VarScaleFillerPolicy, ScaleMode, FanIn);
    TypePolicyObj(PFanOutVarScale, VarScaleFillerPolicy, ScaleMode, FanOut);
    TypePolicyObj(PFanAvgVarScale, VarScaleFillerPolicy, ScaleMode, FanAvg);
}

#include "../../../policies/policy_macro_end.h"
