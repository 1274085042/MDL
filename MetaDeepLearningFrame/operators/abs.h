#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "../data_type/facilities/traits.h"

namespace MDL
{
    template <typename T>
    struct OperAbs_
    {
    private:
        using RawType = RemConstRef<T>;
        
    public:
        static constexpr bool valid = IsMatrix<RawType> || IsMatrixBatch<RawType> ;

        static auto Eval(T && t)
        {
            using ResType = UnaryOp<UnaryOpTags::Abs, RawType>;
            return ResType(std::forward<T> (t));
        }
    };

    template<typename T, std::enable_if_t<OperAbs_<T>::valid> * = nullptr>
    auto Abs(T&&t)
    {
        return OperAbs_<T>::Eval(std::forward<T>(t));
    }
}