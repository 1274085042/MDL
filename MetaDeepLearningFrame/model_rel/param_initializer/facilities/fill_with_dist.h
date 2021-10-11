#pragma once
#include "../../../data_type/matrices/cpu_matrix.h"
#include "../../../data_type/facilities/lower_access.h"
#include <exception>

namespace MDL
{
    namespace NSInitializer
    {
        template<typename tElem, typename tDistribution, typename tEngine>
        void Fill(Matrix<tElem, DeviceTags::CPU> &data, tDistribution &dis, tEngine &eng)
        {
            if(!data.AvailabalForWrite())
            {
                throw std::runtime_error("Matrix is sharing weight, cannot fill");
            }

            const unsigned rowNum = data.RowNum();
            const unsigned colNum = data.ColNum();
            const unsigned rowLen = rowNum;
            auto la = LowerAccess(data);
            auto p = la.MutableRawMemory();

            for(unsigned i=0; i<rowNum;++i)
            {
                for(unsigned j=0; j<colNum; ++j)
                {
                    p[j] = static_cast<tElem>(dis(eng));
                }
                p+=rowLen;
            }
        }
    }
}