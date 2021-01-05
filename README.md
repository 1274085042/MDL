# MetaDeepLearningFrame  
## 命名方式  
* 自定义的函数和类型首字母大写   
* 变量和参数使用匈牙利命名方式

## MetaDeepLearningFrame/facilities/var_type_dict.h  
异类词典VarTypeDict：键是编译时期常量，只是运行时期对象  
**VarTypeDict和map相比的优缺点：**  
优点，map需要通过键的比较，来设置插入位置，这个过程是在运行期完成的，VarTypeDict中的[`Set()`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/facilities/var_type_dict.h#L96)和[`Get()`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/facilities/var_type_dict.h#L118)函数
也有这种查找工作，相应的代码为 ```constexpr static size_t pos = Key2Pos<tKey_, tKey...>;``` 这是在编译时期完成的，并不占用运行期的时间。  
缺点，异类词典的元素个数是固定的，不能在运行期像map一样添加和删除元素


![][image1]

[//]: # (reference)  
[image1]: ./Explanation/VarTypeDict.png