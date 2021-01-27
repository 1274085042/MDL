# MetaDeepLearningFrame  
## 命名方式  
* 自定义的函数和类型首字母大写   
* 变量和参数使用匈牙利命名方式

## 1 异类词典和policy
### MetaDeepLearningFrame/facilities/var_type_dict.h  
异类词典VarTypeDict：键是编译时期常量，只是运行时期对象  
**VarTypeDict和map相比的优缺点：**  
优点，map需要通过键的比较，来设置插入位置，这个过程是在运行期完成的，VarTypeDict中的[`Set()`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/facilities/var_type_dict.h#L96)和[`Get()`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/facilities/var_type_dict.h#L118)函数
也有这种查找工作，相应的代码为 ```constexpr static size_t pos = Key2Pos<tKey_, tKey...>;``` 这是在编译时期完成的，并不占用运行期的时间。  
缺点，异类词典的元素个数是固定的，不能在运行期像map一样添加和删除元素    
#### 1.1 异类词典结构
![][image1]  

### MetaDeepLearningFrame/policies/policy_selector.h  
VarTypeDict和policy本质上都是容器，都是通过键来索引容器中的值  
与异类词典相比， policy模板的键与值都是编译时期常量    

#### 1.2 policy继承体系
![][image2]

## 2 数据类型
### MetaDeepLearningFrame/data_type/scalar/scalar.h
#### 2.1 Scalar 标量类型  
![][image3]  

### MetaDeepLearningFrame/data_type/matrices/cpu_matrix.h
#### 2.2 Matrix 矩阵类型
![][image4]  

**父子矩阵中数据成员之间的关系**
![][image5]

### MetaDeepLearningFrame/data_type/matrices/trival_matrix.h
#### 2.3 平凡矩阵  
![][image6]
### MetaDeepLearningFrame/data_type/matrices/zero_matrix.h
#### 2.4 零矩阵  
![][image7]  

### MetaDeepLearningFrame/data_type/matrices/one_hot_vector.h    
#### 2.5 独热向量  
![][image8]    

注意：引入新的矩阵类需要做的事情  
* 定义一个新的模板类，存储矩阵的行数和列数
* 在模板类中引入相应的接口，包括表示计算单元和计算设备的接口、与求值相关的接口、以及返回行数和列数的接口  
* 对IsMatrix进行特化，来刻画矩阵类型和矩阵类别之间的关系   


### MetaDeepLearningFrame/data_type/batch/matrix_batch.h  
#### 2.6 矩阵Batch
![][image9]  

### MetaDeepLearningFrame/data_type/batch/cus_vector.h 
#### 2.8 矩阵（Matrix）vector和标量（Scalar）vector  
![][image10]

[//]: # (reference)  
[image1]: ./Explanation/VarTypeDict.png 
[image2]: ./Explanation/policy.png
[image3]: ./Explanation/Scalar.png
[image4]: ./Explanation/CPU_Matrix.png
[image5]: ./Explanation/SubMatrix.png
[image6]: ./Explanation/Trival_Matrix.png
[image7]: ./Explanation/Zero_Matrix.png
[image8]: ./Explanation/OneHot_Vector.png
[image9]: ./Explanation/Matrix_Batch.png
[image10]: ./Explanation/Cus_Vector.png