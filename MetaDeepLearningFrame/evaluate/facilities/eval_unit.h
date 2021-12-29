#pragma once

/*
* EvalUnit包含了具体的求值计算逻辑。
* 所有的EvalUnit均派生自BaseEvalUnit
*/
namespace MDL
{
    template <typename tDevice>
    class BaseEvalUnit
    {
    public:
        using DeviceType = tDevice;
        virtual void Eval() = 0;
        virtual ~BaseEvalUnit() = default;
    };
}