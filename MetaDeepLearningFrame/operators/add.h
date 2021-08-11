#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "../data_type/batch/duplicate.h"
#include "../data_type/facilities/traits.h"
#include "../data_type/matrices/trival_matrix.h"
#include <type_traits>

using namespace MDL;

template <typename tP1, typename tP2>
struct  OperAdd_
{
private:
    using RawType1 = RemConstRef<tP1>;
    using RawType2 = RemConstRef<tP2>;

public:
    static constexpr bool valid = (IsMatrix<RawType1> && IsMatrix<RawType2>) ||
                                  (IsMatrix<RawType1> && IsScalar<RawType2>) ||
                                  (IsScalar<RawType1> && IsMatrix<RawType2>) ||
                             (IsMatrixBatch<RawType1> && IsMatrix<RawType2>) ||
                             (IsMatrix<RawType1> && IsMatrixBatch<RawType2>) ||
                        (IsMatrixBatch<RawType1> && IsMatrixBatch<RawType2>) ||
                             (IsMatrixBatch<RawType1> && IsScalar<RawType2>) ||
                             (IsScalar<RawType1> && IsMatrixBatch<RawType2>);

    template <typename T1, typename T2, std::enable_if_t<std::is_same<T1, T2>::value> * =nullptr>
    static auto Eval(tP1 &&t1, tP2 &&t2)
    {
        static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value, 
                     "Matrices with different element types cannot add directly");
        static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value, 
                     "Matrices with different device types cannot add directly");
        
        using ResType = BinaryOp<BinaryOpTags::Add, RawType1, RawType2>;
        return ResType(std::forward<tP1>(t1), std::forward<tP2>(t2));
    }

    template <typename T1, typename T2, 
    std::enable_if_t<std::is_same<T1, CategoryTags::Scalar>::value>* = nullptr,
    std::enable_if_t<std::is_same<T2, CategoryTags::Matrix>::value >* = nullptr>
    static auto Eval(tP1 &&t1, tP2 &&t2)
    {
        using ElementType = typename RawType2::ElementType;
        using DeviceType = typename RawType2::DeviceType;
        auto tmpMatrix = MakeTrivalMatrix<ElementType, DeviceType>(t2.RowNum(), t2.ColNum(), std::forward<tP1>(t1));

        using ResType = BinaryOp<BinaryOpTags::Add, RemConstRef<decltype(tmpMatrix)>, RawType2>;
        return ResType(std::move(tmpMatrix), std::forward<tP2>(t2));
    }

    template <typename T1, typename T2,
    std::enable_if_t<std::is_same<T1, CategoryTags::Matrix>::value>* = nullptr,
    std::enable_if_t<std::is_same<T2, CategoryTags::Scalar>::value>* = nullptr>
    static auto Eval(tP1 &&t1, tP2 &&t2)
    {
        return OperAdd_<tP2, tP1>::template Eval<T2,T1>(std::forward<tP2>(t2), std::forward<tP1>(t1));
    }

    template <typename T1, typename T2, 
    std::enable_if_t<std::is_same<T1, CategoryTags::MatrixBatch>::value> * = nullptr,
    std::enable_if_t<std::is_same<T2, CategoryTags::Matrix>::value> * = nullptr>
    static auto Eval(tP1 &&t1, tP2 &&t2)
    {
        static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value, 
        "Matrices with different element type cannnot add directly");
        static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value, 
        "Matrice with different device type cannot add directly");

        Duplicate<RawType2> tmp = Duplicate<RawType2>(std::forward<tP2>(t2), t1.BatchNum());

        using ResType = BinaryOp<BinaryOpTags::Add, RawType1, Duplicate<RawType2>>;
        return ResType(std::forward<tP1>(t1), std::move(tmp));
    }

    template <typename T1, typename T2,
    std::enable_if_t<std::is_same<T1, CategoryTags::Matrix>::value> * = nullptr,
    std::enable_if_t<std::is_same<T2, CategoryTags::MatrixBatch>::value> * = nullptr>
    static auto Eval(tP1 &&t1, tP2 &&t2)
    {
        //template指明Eval是一个函数模板，从而让编译器将Eval后面的<解释成为模板参数开头而不是小于号
        return OperAdd_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(t2), std::forward<tP1>(t1));
    }

    template <typename T1, typename T2, 
    std::enable_if_t<std::is_same<T1, CategoryTags::Scalar>::value> *=nullptr,
    std::enable_if_t<std::is_same<T2, CategoryTags::MatrixBatch>::value> *=nullptr>
    static auto Eval(tP1 &&t1, tP2 &&t2)
    {
        using ElementType = typename RawType2::ElementType;
        using DeviceType = typename RawType2::DeviceType;

        auto tmpMatrix = MakeTrivalMatrix<ElementType, DeviceType>(t2.RowNum(), t2.ColNum(), std::forward<tP1>(t1));

        Duplicate<RawType2> tmpMatrixBatch = Duplicate<RawType2>(std::move(tmpMatrix), t2.BatchNum());

        using ResType = BinaryOp<BinaryOpTags::Add, Duplicate<RawType2> ,RawType2>;

        return ResType(std::move(tmpMatrixBatch), std::forward<tP2>(t2));
    }

    template <typename T1, typename T2,
    std::enable_if_t<std::is_same<T1, CategoryTags::MatrixBatch>::value> * =nullptr,
    std::enable_if_t<std::is_same<T2, CategoryTags::Scalar>::value> * =nullptr>
    static auto Eval(tP1 &&t1, tP2 &&t2)
    {
        return OperAdd_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(t2), std::forward<tP1>(t1))
    }
     
};

template <typename tP1, typename tP2,
std::enable_if_t<OperAdd_<tP1, tP2>::valid> * = nullptr>
auto operator + (tP1 && t1, tP2 &&t2)
{
    using Cate1 = typename DataCategory<tP1>;
    using Cate2 = typename DataCategory<tP2>;

    return OperAdd_<tP1, tP2>::template Eval<Cate1, Cate2>(std::forward<tP1>(t1), std::forward<tP2>(t2));
}