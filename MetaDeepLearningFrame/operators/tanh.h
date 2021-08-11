#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "../data_type/facilities/traits.h"
#include <type_traits>

namespace MDL
{
    template<typename tP>
    struct OperTanh_
    {
    private:
        using RawType =  RemConstRef<tP>;

    public:
        static constexpr bool valid = IsMatrix<RawType> || IsMatrixBatch<RawType>;

        static auto Eval(tP &&tp)
        {
            using ResType = UnaryOp<UnaryOpTags::Tanh, RawType>;
            return ResType(std::forward<tP>(tp));
        }
    };

    template<typename tP, std::enable_if_t<OperTanh_<tP>::valid> * = nullptr>
    auto Tanh(tP &&tp)
    {
        return OperTanh_<tP>::Eval(std::forward<tP>(tp));
    }
}