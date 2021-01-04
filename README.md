# MetaDeepLearningFrame  

## MetaDeepLearningFrame/facilities/var_type_dict.h  
**VarTypeDict和map相比的优势：**  
map需要通过键的比较，来设置插入位置，这个过程是在运行期完成的，VarTypeDict中的[`Set()`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/facilities/var_type_dict.h#L96)和[`Get()`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/facilities/var_type_dict.h#L118)函数
也有这种查找工作，相应的代码为 ```constexpr static size_t pos = Key2Pos<tKey_, tKey...>;``` 这是在编译时期完成的，并不占用运行期的时间。


![][image1]

[//]: # (reference)  
[image1]: ./Explanation/VarTypeDict.png