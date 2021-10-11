#pragma once

#include "../data_type/matrices/cpu_matrix.h"
#include <stdexcept>

namespace MDL
{
    template<typename tElem>
    void DataCopy(const Matrix<tElem, DeviceTags::CPU> &srcM, Matrix<tElem, DeviceTags::CPU> &destM)
    {
        const size_t rowNum = srcM.RowNum();
        const size_t colNum = srcM.ColNum();

        if((rowNum!= destM.RowNum()) || (colNum != destM.ColNum()))
        {
           throw std::runtime_error("Error DataCopy(): Matrix dimension mismatch");
        }

        auto srcMem = LowerAccess(srcM);
        auto destMem = LowerAccess(destM);

        const auto srcPointer = srcM.RawMemory();
        auto destPointer = srcM.MutableRawMemory();

        memcpy(destPointer, srcPointer, sizeof(tElem)*rowNum*colNum);
    }
}
