#pragma once

#include "./facilities/traits.h"
#include "../../facilities/traits.h"
#include "../../facilities/data_copy.h"
#include "../../policies/policy_container.h"
#include "../../data_type/matrices/cpu_matrix.h"
#include <map>
#include <string>
#include <stdexcept>

namespace MDL
{
    template<typename tElem, typename tPolicyCont, typename tFillers>
    struct ParamInitializer
    {
    private:
        tFillers fillers_; //VarTypeDict::Values<>实例，用于保存初始化器对象
        std::map<std::string, Matrix<tElem, DeviceTags::CPU>> matrixMap; //矩阵参数
        
    public:
        using PolicyCont = tPolicyCont;
        
        ParamInitializer(tFillers && fillers):fillers_(move(fillers))
        {}

        template<typename tTag, typename tType>
        auto SetFillers(tType &&val) &&
        {
            auto fillers = move(fillers_).template Set<tTag, tType>(std::forward<tType>(val));
            using typeFillers = RemConstRef<decltype(fillers)>;
            return ParamInitializer<tElem, tPolicyCont, typeFillers>(move(fillers));
        }

        template<typename tTag>
        auto& GetFiller()
        {
            return fillers_.template Get<tTag>();
        }

        template<typename tElem2, typename tDevice2>
        void SetMatrix(const string &name, const Matrix<tElem2, tDevice2> &srcMatrix)
        {
            if(matrixMap.find(name) != matrixMap.end())
            {
                throw std::runtime_error("Duplicate matrix: "+ name);
            }

            if constexpr(std::is_same<tElem, tElem2>::value && std::is_same<tDevice2, DeviceTags::CPU>::value)
            {
                matrixMap.insert({name, srcMatrix});
            }
            else
            {
                Matrix<tElem, DeviceTags::CPU> destMatrix(srcMatrix.RowNum(), srcMatrix.ColNum());
                DataCopy(srcMatrix, destMatrix);
                matrixMap.insert({name, move(destMatrix)});
            }
        }
        
        void GetMatrix(const string &name, Matrix<tElem, DeviceTags::CPU> &resMatrix)
        {
            auto iterator = matrixMap.find(name);
            if(iterator == matrixMap.end())
            {
                throw std::runtime_error("Matrix not exist: "+ name);
            }
            else
            {
                auto &srcMatirx=iterator->second();
                if((resMatrix.RowNum()!= srcMatirx.RowNum()) || (resMatrix.ColNum()!=srcMatirx.ColNum()))
                {
                    throw std::runtime_error("Matrices with different dimensions.");
                }
                else
                {
                    DataCopy(srcMatirx, resMatrix);
                }
            }
        }

        bool IsMatrixExist(const string &name)
        {
            return matrixMap.find(name) != matrixMap.end();
        }
    };
}

template <typename tElem, typename ...tPolicies>
auto MakeInitializer()
{
    using npType = FillerTags2NamedParams<tPolicies>;
    using FillDictType = RemConstRef<decltype(npType::Creat())>;
    return ParamInitializer<tElem, PolicyContainer<tPolicies...>, FillDictType>(npType::Creat());
}
