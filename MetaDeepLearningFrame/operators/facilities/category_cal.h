#include "../../data_type/facilities/traits.h"
#include "../../facilities/traits.h"
#include <tuple>

namespace MDL
{
    /*
    * Data2Cate：是把类型全部转换为类别标签存储在tuple<>中
    */
    template <typename tCateCont, typename ...tData>
    struct Data2Cate_
    {
        using type = tCateCont;
    };
    template <typename ...tProcessed, typename tCur, typename ...tRemain>
    struct Data2Cate_<std::tuple<tProcessed...>, tCur, tRemain...>
    {
        using tmp1 = DataCategory<tCur>;
        using tmp2 = std::tuple<tProcessed..., tmp1>;
        using type = typename Data2Cate_<tmp2, tRemain...>::type;
    };
    template <typename tHead, typename ...tRemain>
    using Data2Cate = typename Data2Cate_<std::tuple<>, tHead, tRemain...>::type;
    
    /*
    * SameCate_：判断所有的类别标签是否相等
    */
    template <typename tCateHead, typename ...tCateRemain>
    struct SameCate_
    {
       static constexpr bool value = true;
    };
    template <typename tCate, typename tCateCur, typename ...tCateRemain>
    struct SameCate_<tCate, tCateCur, tCateRemain...>
    {
       static constexpr bool tmp = std::is_same<tCate, tCateCur>::value;
       static constexpr bool value = AndValue<tmp, SameCate_<tCate, tCateRemain...>>;
    };
    
    /*
    * 如果输入参数的所有类别标签都相等
    * 那么运算结果的类别标签就等于第一个输入参数的类别标签
    */
    template <typename tOpTag,typename tCateHead, typename ...tCateRemain>
    struct OperCategory_
    {
        static_assert(SameCate_<tCateHead, tCateRemain...>::value, "Parameter category mismatch");
        using type = tCateHead;
    };
    
    /*
    * CateInduce_：将tuple中的类别标签拿出来
    */
    template <typename tOpTag, typename tCateCont>
    struct CateInduce_;
    template <typename tOpTag, typename ... tCates>
    struct CateInduce_<tOpTag, std::tuple<tCates...>>
    {
        using type = typename OperCategory_<tOpTag, tCates...>::type;
    };

    /*
    * 输入参数类型的类别是否一致
    * 如果都一致得到的OperCateCal为第一个参数类型的类别
    */
    template <typename tOpTag, typename tHead, typename ... tRemain>
    using OperCateCal = typename CateInduce_<tOpTag, Data2Cate<tHead, tRemain...>>::type;
}