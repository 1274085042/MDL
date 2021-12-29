#pragma once

#include "../facilities/eval_pool.h"
#include "../../data_type/facilities/tags.h"

namespace MDL
{
    /*
    * TrivialEvalPool一个平凡的EvalPool，不涉及多线程操作，
    * 其功能是对传入的EvalUnit对象求值
    */
    template<>
    class TrivialEvalPool<DeviceTags::CPU> : public BaseEvalPool<DeviceTags::CPU>
    {
    private:
        TrivialEvalPool() = default;

    public:
        static TrivialEvalPool &Instance()
        {
            static TrivialEvalPool inst;
            return inst;
        }

        void Process(std::shared_ptr<BaseEvalUnit<DeviceTags::CPU>> sp)
        {
            sp->Eval();
        }

        void Barrier()
        {
        }
    };
}