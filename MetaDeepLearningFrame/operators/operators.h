#pragma once

#include "./facilities/category_cal.h"
#include "./facilities/operator_seq.h"
#include "./facilities/organizer.h"
#include "./facilities/traits.h"

/*
* 运算模板的框架
* UnaryOp 
* BinaryOp
* TernaryOp
*/
namespace MDL
{
    template <typename tOpTag, typename tData>
    class UnaryOp: public OperOrganizer<tOpTag, OperCateCal<tOpTag, tData>>
    {
    private:
        static_assert(std::is_same<RemConstRef<tData>, tData>::value, "tData is not an available type");
        using Cate = OperCateCal<tOpTag, tData>;
    
    public:
        using ElementType = typename OperElementType_<tOpTag, tData>::type;
        using DeviceType = typename OperDeviceType_<tOpTag, tData>::type;

    private:
        tData data_;
        using Principal = PrincipalDataType<Cate, ElementType, DeviceType>;

    public:
        UnaryOp(tData data): OperOrganizer<tOpTag, Cate>(data), data_(move(data))
        {}

        bool operator==(const UnaryOp & val) const
        {
            return data_ == val.data_;
        }

        template <typename tOther>
        bool operator==(const tOther &) const
        {
            return false;
        }

        template <typename tOther>
        bool operator!=(const tOther & val) const
        {
            return !(operator==(val));
        }

        const tData & Operand() const
        {
            return data_;
        }

        auto EvalRegister() const
        {

        }
    };
    
    template <typename tOpTag, typename tData1, typename tData2>
    class BinaryOp: public OperOrganizer<tOpTag, OperCateCal<tOpTag, tData1, tData2>>
    {
    private:
        static_assert(std::is_same<RemConstRef<tData1>, tData1>::value, "tData1 is not an available type");
        static_assert(std::is_same<RemConstRef<tData2>, tData2>::value, "tData2 is not an available type");

        using Cate = OperCateCal<tOpTag, tData1, tData2>;  //确保所有类型tData1、tData2的类别一致
    
    public:
        using ElementType = typename OperElementType_<tOpTag, tData1, tData2>::type;
        using DeviceType = typename OperDeviceType_<tOpTag, tData1, tData2>::type;

    private:
        tData1 data1_;
        tData2 data2_;
        using Principal = PrincipalDataType<Cate, ElementType, DeviceType>;

    public:
        BinaryOp(tData1 data1, tData2 data2): OperOrganizer<tOpTag, Cate>(data1, data2), data1_(move(data1)), data2_(move(data2))
        {}

        bool operator==(const BinaryOp & val) const
        {
            return (data1_ == val.data1_) && (data2_ == val.data2_);
        }

        template <typename tOther>
        bool operator==(const tOther &) const
        {
            return false;
        }

        template <typename tOther>
        bool operator!=(const tOther & val) const
        {
            return !(operator==(val));
        }

        const tData1 & Operand1() const
        {
            return data1_;
        }

        const tData2 & Operand2() const
        {
            return data2_;
        }

        auto EvalRegister() const
        {

        }

    };

    template <typename tOpTag, typename tData1, typename tData2, typename tData3>
    class TernaryOp: public OperOrganizer<tOpTag, OperCateCal<tOpTag, tData1, tData2, tData3>>
    {
    private:
        static_assert(std::is_same<RemConstRef<tData1>, tData1>::value, "tData1 is not an available type");
        static_assert(std::is_same<RemConstRef<tData2>, tData2>::value, "tData2 is not an available type");
        static_assert(std::is_same<RemConstRef<tData3>, tData3>::value, "tData3 is not an available type");

        using Cate = OperCateCal<tOpTag, tData1, tData2, tData3>;  //确保所有类型tData1、tData2、tData3的类别一致
    
    public:
        using ElementType = typename OperElementType_<tOpTag, tData1, tData2, tData3>::type;
        using DeviceType = typename OperDeviceType_<tOpTag, tData1, tData2, tData3>::type;

    private:
        tData1 data1_;
        tData2 data2_;
        tData3 data3_;
        using Principal = PrincipalDataType<Cate, ElementType, DeviceType>;

    public:
        TernaryOp(tData1 data1, tData2 data2, tData3 data3): OperOrganizer<tOpTag, Cate>(data1, data2, data3), data1_(move(data1)), data2_(move(data2)), data3_(move(data3))
        {}

        bool operator==(const TernaryOp & val) const
        {
            return (data1_ == val.data1_) && (data2_ == val.data2_) && (data3_ == val.data3_);
        }

        template <typename tOther>
        bool operator==(const tOther &) const
        {
            return false;
        }

        template <typename tOther>
        bool operator!=(const tOther & val) const
        {
            return !(operator==(val));
        }

        const tData1 & Operand1() const
        {
            return data1_;
        }

        const tData2 & Operand2() const
        {
            return data2_;
        }

        const tData3 & Operand3() const
        {
            return data3_;
        }

        auto EvalRegister() const
        {

        }
    };

    template <typename tOpTag, typename tData>
    constexpr bool IsScalar<UnaryOp<tOpTag, tData>> =std::is_same<OperCateCal<tOpTag, tData>, CategoryTags::Scalar>::value;  
    template <typename tOpTag, typename tData>
    constexpr bool IsScalarBatch<UnaryOp<tOpTag, tData>> =std::is_same<OperCateCal<tOpTag, tData>, CategoryTags::ScalarBatch>::value;
    template <typename tOpTag, typename tData>
    constexpr bool IsMatrix<UnaryOp<tOpTag, tData>> =std::is_same<OperCateCal<tOpTag, tData>, CategoryTags::Matrix>::value;
    template <typename tOpTag, typename tData>
    constexpr bool IsMatrixBatch<UnaryOp<tOpTag, tData>> =std::is_same<OperCateCal<tOpTag, tData>, CategoryTags::MatrixBatch>::value;

    template <typename tOpTag, typename tData1, typename tData2>
    constexpr bool IsScalar<BinaryOp<tOpTag, tData1, tData2>> =std::is_same<OperCateCal<tOpTag, tData1, tData2>, CategoryTags::Scalar>::value;  
    template <typename tOpTag, typename tData1, typename tData2>
    constexpr bool IsScalarBatch<BinaryOp<tOpTag, tData1, tData2>> =std::is_same<OperCateCal<tOpTag, tData1, tData2>, CategoryTags::ScalarBatch>::value;
    template <typename tOpTag, typename tData1, typename tData2>
    constexpr bool IsMatrix<BinaryOp<tOpTag, tData1, tData2>> =std::is_same<OperCateCal<tOpTag, tData1, tData2>, CategoryTags::Matrix>::value;
    template <typename tOpTag, typename tData1, typename tData2>
    constexpr bool IsMatrixBatch<BinaryOp<tOpTag, tData1, tData2>> =std::is_same<OperCateCal<tOpTag, tData1, tData2>, CategoryTags::MatrixBatch>::value;

    template <typename tOpTag, typename tData1, typename tData2, typename tData3>
    constexpr bool IsScalar<TernaryOp<tOpTag, tData1, tData2, tData3>> =std::is_same<OperCateCal<tOpTag, tData1, tData2, tData3>, CategoryTags::Scalar>::value;
    template <typename tOpTag, typename tData1, typename tData2, typename tData3>
    constexpr bool IsScalarBatch<TernaryOp<tOpTag, tData1, tData2, tData3>> =std::is_same<OperCateCal<tOpTag, tData1, tData2, tData3>, CategoryTags::ScalarBatch>::value;
    template <typename tOpTag, typename tData1, typename tData2, typename tData3>
    constexpr bool IsMatrix<TernaryOp<tOpTag, tData1, tData2, tData3>> =std::is_same<OperCateCal<tOpTag, tData1, tData2, tData3>, CategoryTags::Matrix>::value;
    template <typename tOpTag, typename tData1, typename tData2, typename tData3>
    constexpr bool IsMatrixBatch<TernaryOp<tOpTag, tData1, tData2, tData3>> =std::is_same<OperCateCal<tOpTag, tData1, tData2, tData3>, CategoryTags::MatrixBatch>::value;
}