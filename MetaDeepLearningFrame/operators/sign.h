#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "../data_type/facilities/traits.h"
#include <type_traits>

namespace MDL
{
    template <typename tP>
    struct OperSign_
    {
    private:
        using RawType = RemConstRef<tP>;
    
    public:
        static constexpr bool valid = IsMatrix<RawType> || IsMatrixBatch<RawType>;

        static auto Eval(tP &&tp)
        {
            using ResType = UnaryOp<UnaryOpTags::Sign, RawType>;
            return ResType(std::forward<tP>(tp));
        }
    };

    template <typename tP, std::enable_if_t<OperSign_<tP>::valid> * = nullptr>
    auto Sign(tP &&tp)
    {
        return OperSign_<tP>::Eval(std::forward<tP>(tp));
    }
}