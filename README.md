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
### 异类词典结构
![][image1]
## MetaDeepLearningFrame/policies/policy_selector.h  
VarTypeDict和policy本质上都是容器，都是通过键来索引容器中的值  
与异类词典相比， policy模板的键与值都是编译时期常量    

### policy继承体系
![][image2]

## 数据类型
### MetaDeepLearningFrame/data_type/scalar/scalar.h
#### Scalar 标量类型  
![][image3]  

### MetaDeepLearningFrame/data_type/matrices/cpu_matrix.h
#### Matrix 矩阵类型
![][image4]  

**父子矩阵中数据成员之间的关系**
![][image5]

### MetaDeepLearningFrame/data_type/matrices/trival_matrix.h
#### 平凡矩阵  
![][image6]
### MetaDeepLearningFrame/data_type/matrices/zero_matrix.h
#### 零矩阵  
![][image7]  

### MetaDeepLearningFrame/data_type/matrices/one_hot_vector.h    
#### 独热向量  
![][image8]


[//]: # (reference)  
[image1]: ./Explanation/VarTypeDict.png 
[image2]: ./Explanation/policy.png
[image3]: ./Explanation/Scalar.png
[image4]: ./Explanation/CPU_Matrix.png
[image5]: ./Explanation/SubMatrix.png
[image6]: ./Explanation/Trival_Matrix.png
[image7]: ./Explanation/Zero_Matrix.png
[image8]: ./Explanation/OneHot_Vector.png


