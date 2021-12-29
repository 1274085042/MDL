#pragma once

#include "./eval_unit.h"

#include <memory>

namespace MDL
{
    enum class EvalPoolEnum
    {
        Trival
    };

    /*
    * EvalPool获取EvalPlan中的求值请求，调用求值函数完成求值计算
    */
    template<typename tDevice>
    class BaseEvalPool
    {
    public:
        //Barrier()用于等待EvalPool中的所有求值完成
        virtual void Barrier() = 0;
        virtual void Process(std::shared_ptr<BaseEvalUnit<tDevice>> ) = 0;
        virtual ~BaseEvalPool() = default;
    };

    template <typename tDevice>
    class TrivialEvalPool;

}