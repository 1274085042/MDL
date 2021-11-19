#pragma once
#include "../../data_type/facilities/tags.h"
#include "../../policies/policy_macro_begin.h"

namespace MDL
{
    //更新参数和反向传播相关参数
    struct  FeedbackPolicy
    {
        using MajorClass = FeedbackPolicy;

        struct IsUpdateValueCate;
        static constexpr bool IsUpdate = false; //表示是否对某个层的参数进行更新
        
        struct IsFeedbackOutputValueCate;
        static constexpr bool IsFeedbackOutput = false; //是否输出梯度
    };
    ValuePolicyObj(PUpdate, FeedbackPolicy, IsUpdate, true);
    ValuePolicyObj(PNoUpdate, FeedbackPolicy, IsUpdate, false);
    ValuePolicyObj(PFeedbackOutput, FeedbackPolicy, IsFeedbackOutput, true);
    ValuePolicyObj(PNoFeedbackOutput, FeedbackPolicy, IsFeedbackOutput, false);

    //运算相关参数
    struct OperandPolicy
    {
        using MajorClass = OperandPolicy;

        struct DeviceTypeCate:public DeviceTags
        {};  
        using Device = DeviceTags::CPU;

        struct ElementTypeCate;
        using Element = float;
    };
    TypePolicyObj(PCPUDevice, OperandPolicy, Device, CPU);
    TypePolicyTemplate(PElementTypeIs, OperandPolicy, Element);

    //输入相关参数
    struct InputPolicy
    {
        using MajorClass = InputPolicy;

        struct BatchModeValueCate;
        constexpr static bool BatchMode = false; //是否接受列表形式的输入
    };
    ValuePolicyObj(PBatchMode, InputPolicy, BatchMode, true);
    ValuePolicyObj(PNoBatchMode, InputPolicy, BatchMode, false);
}

#include "../../policies/policy_macro_end.h"