#pragma once

#include "./eval_handle.h"
#include <memory>

namespace MDL
{
    /*
    * EvalBuffer保存了求值结果，避免对相同的对象反复求值、
    * Handle()：返回句柄，用于修改求值结果
    * ConstHandle()：返回句柄，用于获取求值结果
    */
    template<typename tData>
    class EvalBuffer
    {
    private:
        EvalHandle<tData> handle_;
    
    public:
        using DataType = tData;

        auto Handle()
        {
            return handle_;
        }

        auto ConstHandle()
        {
            return ConstEvalHandle<EvalHandle<tData>>(handle_);
        }

        bool IsEvaluated()
        {
            return handle_.IsEvaluated();
        }
    };
}