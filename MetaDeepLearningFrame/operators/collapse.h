#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "./facilities/organizer.h"
#include "../data_type/facilities/traits.h"

namespace MDL
{
    template <>
    struct OperCategory_<UnaryOpTags::Collapse, CategoryTags::MatrixBatch> 
    {
        using type = CategoryTags::Matrix;
    };
    
    template <typename T>
    struct  OperCollapse_
    {
    private:
        using RawType = std::decay_t<T>;
    
    public:
        static constexpr bool vaild = IsMatrixBatch<RawType>;

        static auto Eval(T &&t)
        {
            using ResType = UnaryOp<UnaryOpTags::Collapse, RawType>;
            return ResType(std::forward<T> (t))
        }
    };
    
    template <typename T, std::enable_if_t<OperCollapse_<T>::vaild> * = nullptr>
    auto Collapse(T &&t)
    {
        return OperCollapse_<T>::Eval(std::forward<T> (t));
    }
}