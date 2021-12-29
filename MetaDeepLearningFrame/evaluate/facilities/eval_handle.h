#pragma once

#include <memory>
#include <stdexcept>

namespace MDL
{

    /*
    * EvalHandle 求值句柄
    * Data接口，返回求值结果
    * DataPtr接口，指向求值结果的指针
    * 接口IsEvaluated判断数据的有效性（是否已经进行过求值），如果该接口返回false，
    * 那么就需要进行求值并将结果填充到句柄之中。
    * Allocate()分配保存结果的对象，MutableData()获取结果对象的引用进行填充，
    * SetEval()在填充完结果后设置数据的有效性。
    */
    template <typename tData>
    class EvalHandle
    {
    private:
        struct DataWithEvalInfo
        {
            tData data;
            bool eval = false;
        };

        std::shared_ptr<DataWithEvalInfo> data_;
    
    public:
        EvalHandle():data_(std::make_shared<DataWithEvalInfo>())
        {}

        bool IsEvaluated()
        {
            return data_->eval;
        }

        void SetEval()
        {
            if(IsEvaluated())
            {
                throw std::runtime_error("Data is already evaluated.");
            }

            data_->eval = true;
        }

        template<typename ...tParams>
        void Allocate(tParams &&...params)
        {
            if(IsEvaluated())
            {
                throw std::runtime_error("Data is already evaluated.");
            }

            data_->data = tData(std::forward<tParams>(params)...);  //注意变长参数的语法
        }

        tData &MutableData()
        {
            if(IsEvaluated())
            {
                throw std::runtime_error("Data is already evaluated.");
            }

            return data_->data;
        }
        
        const tData &Data()
        {
            if(!IsEvaluated())
            {
                throw std::runtime_error("Data is not evaluated.");
            }

            return data_->data;
        }

        const void * DataPtr()
        {
            return data_.get();
        }

    };

    /*
    * MDL中的每个数据类型都提供EvalRegister接口，用于注册求值，返回相应的句柄。
    * 这个句柄用于获取求值结果，无需提供修改接口，
    * 因此使用ConstEvalHandle来刻画这种只读句柄。
    */
    template<typename tData>
    class ConstEvalHandle
    {
    private:
        tData constData_;
    
    public:
        ConstEvalHandle(tData data):constData_(move(data))
        {}

        const tData& Data()
        {
            return constData_;
        }

        const void *DataPtr()
        {
            return &constData_;
        }
    };

    template<typename tData>
    class ConstEvalHandle<EvalHandle<tData>>
    {
    private:
        EvalHandle<tData> constData_;
    
    public:
        ConstEvalHandle(EvalHandle<tData> data):constData_(std::move(data))
        {}

        const tData& Data()
        {
            return constData_.Data();
        }

        const void * DataPtr()
        {
            return constData_.DataPtr();
        }
    };

    template<typename tData>
    auto MakeCosntEvalHandle(const tData &data)
    {
        return ConstEvalHandle<tData>(data);
    }

    template<typename tData>
    class DynamicHandleDataBase
    {
    public:
        virtual const tData &Data() = 0;
        virtual const void *DataPtr() = 0;
        virtual ~DynamicHandleDataBase() = default;
    };

    template <typename tData>
    class DynamicHandleData;

    /*
    * 偏特化的派生类
    */
    template <typename tData>
    class DynamicHandleData<ConstEvalHandle<tData>> :public DynamicHandleDataBase<tData>
    {
    private:
        ConstEvalHandle<tData> data_;

    public:
        DynamicHandleData(ConstEvalHandle<tData> data):DynamicHandleDataBase<tData>(), data_(move(data))
        {}

        const tData& Data()
        {
            return data_.Data();
        }

        const void* DataPtr()
        {
            return data_.DataPtr();
        }
    };

    template<typename tData>
    class DynamicHandleData<ConstEvalHandle<EvalHandle<tData>>> :public DynamicHandleDataBase<tData>
    {
    private:
        ConstEvalHandle<EvalHandle<tData>> data_;

    public:
        DynamicHandleData(ConstEvalHandle<EvalHandle<tData>> data):DynamicHandleDataBase<tData>(), data_(move(data))
        {}

        const tData& Data()
        {
            return data_.Data();
        }

        const void *DataPtr()
        {
            return data_.DataPtr();
        }
    };

    /*
    * 对于DynamicData数据类型，由于隐藏了具体的类型信息，
    * 因此在调用EvalRegister时，无法得知构造哪种类型的句柄，
    * 引入DynamicConstEvalHandle，DynamicData::EvalRegister
    * 将返回这个类型的对象
    */
    template<typename tData>
    class DynamicConstEvalHandle
    {
    private:
        using BaseDataType = DynamicHandleDataBase<tData>;
        std::shared_ptr<BaseDataType> data_;

    public:
        template<typename tRealHandle>
        DynamicConstEvalHandle(tRealHandle data):data_(std::make_shared<DynamicHandleData<tRealHandle>>(move(data)))
        {}

        const tData&Data()
        {
            return data_->Data();
        }

        const void *DataPtr()
        {
            return data_->DataPtr();
        }
    };
}