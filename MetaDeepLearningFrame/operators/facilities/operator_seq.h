#pragma once

/*
* OperSeq用来封装求值相关的逻辑，这个元函数会调用一系列辅助元函数，
* 一起实现对某个运算模板的求值
* 每种运算都会引入相应的OperSeq特化，每个OperSeq特化都会封装若干求值方法，
* 每种求值方法都对应一种计算情形，程序会在编译期和运行期根据上下文，
* 选择合适的求值方法——这都是确保求值的高效性
*/
namespace MDL
{
    template <typename ...tCases>
    struct  OperSeqContainer;
    
    template <typename tOpTag>
    struct OperSeq_;
}