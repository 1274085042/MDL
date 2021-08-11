#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "../data_type/facilities/traits.h"
#include <type_traits>

using namespace MDL;

template <typename T>
struct  OperSigmoid_
{
private:
    using RawType = RemConstRef<T>;

public:
    static constexpr bool vaild = IsMatrix<RawType> || IsMatrixBatch<RawType>;

    static auto Eval(T&& t)
    {
        using ResType = UnaryOp<UnaryOpTags::Sigmoid, RawType>;
        return ResType(std::forward<T>(t));
    }
};

template <typename T, std::enable_if_t<OperSigmoid_<T>::vaild> * = nullptr>
auto Sigmoid(T && t)
{
    return OperSigmoid_<T>::Eval(std::forward<T>(t));
}