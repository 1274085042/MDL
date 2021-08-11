#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "../data_type/batch/duplicate.h"
#include "../data_type/facilities/traits.h"
#include "../data_type/matrices/trival_matrix.h"
#include <type_traits>

namespace MDL
{
    template <typename tGrad, typename tOut>
    struct OperSoftmaxDerivative_
    {
    private:
        using RawGrad = RemConstRef<tGrad>;
        using RawOut = RemConstRef<tOut>;

    public:
        static constexpr bool valid = (IsMatrix<RawGrad> && IsMatrix<RawOut>) ||
                            (IsMatrixBatch<RawGrad> && IsMatrixBatch<RawOut>);
        
        static auto Eval(tGrad && grad, tOut && out)
        {
            static_assert(std::is_same<typename RawGrad::ElementType, typename RawOut::ElementType>::value, 
            "Matrices with different element types cannot softmax derivative directly");
            static_assert(std::is_same<typename RawGrad::DeviceType, typename RawOut::DeviceType>::value, 
            "Matrices with different device types cannot softmax derivative directly");

            using ResType = BinaryOp<BinaryOpTags::VecSoftmaxDerivative, RawGrad, RawOut>;
            return ResType(std::forward<tGrad>(grad), std::forward<tOut>(out))
        }

    };

    template<typename tGrad, typename tOut, std::enable_if_t<OperSoftmaxDerivative_<tGrad, tOut>::valid> * = nullptr>
    auto SoftmaxDerivative(tGrad && grad, tOut && out)
    {
        return OperSoftmaxDerivative_<tGrad, tOut>::Eval(std::forward<tGrad>(grad), std::forward<tOut>(out));
    }
}
