#pragma once

#include "../../data_type/matrices/cpu_matrix.h"
#include "../../data_type/facilities/lower_access.h"
#include <exception>

namespace MDL
{
    namespace NSConstantFiller
    {
        template<typename tElem>
        void Fill(Matrix<tElem, DeviceTags::CPU> &mat, const double &val)
        {
            if(!mat.AvailabalForWrite())
            {
                throw std::runtime_error("Matrix is sharing weight, cannot fill");
            }

            const size_t rowNum = mat.RowNum();
            const size_t colNum = mat.ColNum();
            const size_t rowLen = colNum;

            auto la = LowerAccess(mat);
            auto p = la.MutableRawMemory();

            for(size_t i=0;i<rowNum;++i)
            {
                for(size_t j=0;j<colNum;++j)
                {
                    p[j] = static_cast<tElem>(val);
                }
                p+=rowLen;
            }
        }
    }

    class ConstantFiller
    {
    private:
        double val_;

    public:
        ConstantFiller(const double & val=0):val_(val)
        {}

        template<typename tData>
        void Fill(tData &mat)
        {
            NSConstantFiller::Fill(mat, val_);
        }
    };
    
} // namespace MDL
