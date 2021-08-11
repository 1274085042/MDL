#pragma once

#include "../../data_type/facilities/traits.h"

namespace MDL
{
    /*
    * OperOrganizer类模板用于提供尺寸相关的接口
    */
    template <typename tOpTag, typename tCate>
    class OperOrganizer;

    /*
    * 因为不同的数据类别表示尺寸信息的接口也不同
    * 所以OperOrganizer需要针对不同的数据类别进行特化
    */
    template <typename tOpTag>
    class OperOrganizer<tOpTag, CategoryTags::Scalar>
    {
    public:
        template <typename tHead, typename... tRemain>
        OperOrganizer(const Head &, const tRemain &...)
        {
        }
    };

    template <typename tOpTag>
    class OperOrganizer<tOpTag, CategoryTags::ScalarBatch>
    {
    private:
        size_t batchNum_;

        template <typename tHead, typename... tRemain>
        bool SameDim(const tHead &, const tRemain &...)
        {
            return true;
        }
        template <typename tHead, typename tCur, typename... tRemain>
        bool SameDim(const tHead &head, const tCur &cur, const tRemain &...rem)
        {
            bool tmp = (head.BatchNum() == cur.BarchNum());
            return tmp && SameDim(cur, rem...);
        }

    public:
        template <typename tHead, typename... tRemain>
        OperOrganizer(const tHead &head, const tRemain &...rem) : batchNum_(head.BatchNum())
        {
            assert((SameDim(head, rem...)));
        }

        size_t BatchNum() const
        {
            return batchNum_;
        }
    };

    template <typename tOpTag>
    class OperOrganizer<tOpTag, CategoryTags::Matrix>
    {
    private:
        size_t rowNum_;
        size_t colNum_;

        template <typename tHead, typename... tRemain>
        bool SameDim(const tHead &, const tRemain &...)
        {
            return true;
        }
        template <typename tHead, typename tCur, typename... tRemain>
        bool SameDim(const tHead &head, const tCur &cur, const tRemain &... rem)
        {
            bool tmp = (head.RowNum() == cur.RowNum()) && (head.ColNum() == cur.ColNum());
            return tmp && SameDim(head, rem...);
        }

    public:
        template <typename tHead, typename... tReamin>
        OperOrganizer(const tHead &head, const tReamin &...rem) : rowNum_(head.RowNum()), colNum_(head, ColNum())
        {
            assert((SameDim(head, rem...)));
        }

        size_t RowNum() const
        {
            return rowNum_;
        }

        size_t ColNum() const
        {
            return colNum_;
        }
    };

    template <typename tOpTag>
    class OperOrganizer<tOpTag, CategoryTags::MatrixBatch>
    {
    private:
        size_t rowNum_;
        size_t colNum_;
        size_t batchNum_;

        template <typename tHead, typename... tRemain>
        bool SameDim(const tHead &, const tRemain &...)
        {
            return true;
        }
        template <typename tHead, typename tCur, typename... tRemain>
        bool SameDim(const tHead &head, const tCur &cur, const tRemain &...rem)
        {
            bool tmp = (head.RowNum() == cur.RowNum()) && (head.ColNum() == cur.ColNum()) && (head.BatchNum() == cur.BatchNum());
            return tmp && SameDim(head, rem...);
        }

    public:
        template <typename tHead, typename... tRemain>
        OperOrganizer(const tHead &head, const tRemain &...rem) : rowNum_(head.RowNum()), colNum_(head.ColNum()), batchNum_(head.BatchNum())
        {
            assert((SameDim(head, rem...)));
        }

        size_t RowNum()
        {
            return rowNum_;
        }

        size_t ColNum()
        {
            return colNum_;
        }

        size_t BatchNum()
        {
            return batchNum_;
        }
    };
}