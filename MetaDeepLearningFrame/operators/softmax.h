#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "./facilities/organizer.h"
#include "../data_type/facilities/traits.h"

namespace MDL
{
    template<typename tP>
    struct OperSoftmax_
    {
    private:
        using RawType = RemConstRef<tP>;
    
    public:
        static constexpr bool valid = IsMatrix<RawType> || IsMatrixBatch<RawType>;

        static auto Eval(tP && tp)
        {
            using ResType = UnaryOp<UnaryOpTags::VecSoftmax, RawType>;
            return ResType(std::forward<tP>(tp));
        }
    };

    template<typename tP, std::enable_if_t<OperSoftmax_<tP>::valid> * = nullptr>
    auto Softmax(tP && tp)
    {
        return OperSoftmax_<tP>::Eval(std::forward<tP>(tp));
    }
}