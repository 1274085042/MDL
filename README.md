# MetaDeepLearningFrame  
## 命名方式  
* 自定义的函数和类型首字母大写   
* 变量和参数使用匈牙利命名方式

## 1 异类词典和policy
### 1.1 MetaDeepLearningFrame/facilities/var_type_dict.h  
异类词典VarTypeDict：键是编译时期常量，值是运行时期对象  
**VarTypeDict和map相比的优缺点：**  
优点：
- map需要通过键的比较，来设置插入位置，这个过程是在运行期完成的，VarTypeDict中的[`Set()`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/facilities/var_type_dict.h#L96)和[`Get()`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/facilities/var_type_dict.h#L118)函数
也有这种查找工作，相应的代码为 ```constexpr static size_t pos = Key2Pos<tKey_, tKey...>;``` 这是在编译时期完成的，并不占用运行期的时间。  
- 存储的值类型可以是不同的   

缺点：  
异类词典的元素个数是固定的，不能在运行期像map一样添加和删除元素      

使用map和异类词典共同的好处：  
减少了函数调用时的参数误用，如果函数使用异类词典或map作为参数，就相当于使用了具名参数，因为在函数内部需要通过键来获取相应的值     

#### 异类词典结构
![][image1]  

### 1.2 MetaDeepLearningFrame/policies/policy_selector.h    
policy：带有键值对，作用于模板并改变模板行为的类  

VarTypeDict和policy本质上都是容器，都是通过键来索引容器中的值  
与异类词典相比， policy模板的键与值都是编译时期常量    
  
#### policy继承体系
![][image2]

## 2 数据类型
### 2.1 MetaDeepLearningFrame/data_type/scalar/scalar.h
#### Scalar 标量类型  
![][image3]  

### 2.2 MetaDeepLearningFrame/data_type/matrices/cpu_matrix.h
#### Matrix 矩阵类型
![][image4]  

**父子矩阵中数据成员之间的关系**
![][image5]

### 2.3 MetaDeepLearningFrame/data_type/matrices/trival_matrix.h
#### 平凡矩阵  
![][image6]
### 2.4 MetaDeepLearningFrame/data_type/matrices/zero_matrix.h
#### 零矩阵  
![][image7]  

### 2.5 MetaDeepLearningFrame/data_type/matrices/one_hot_vector.h    
#### 独热向量  
![][image8]    

注意：引入新的矩阵类需要做的事情  
* 定义一个新的模板类，存储矩阵的行数和列数
* 在模板类中引入相应的接口，包括表示计算单元和计算设备的接口、与求值相关的接口、以及返回行数和列数的接口  
* 对IsMatrix进行特化，来刻画矩阵类型和矩阵类别之间的关系   

### 2.6 MetaDeepLearningFrame/data_type/batch/scalar_batch.h  
#### 标量Batch  
![][image12]

### 2.7 MetaDeepLearningFrame/data_type/batch/matrix_batch.h  
#### 矩阵Batch
![][image9]  

### 2.8 MetaDeepLearningFrame/data_type/batch/cus_vector.h 
#### 矩阵（Matrix）vector和标量（Scalar）vector  
![][image10]

### 2.9 MetaDeepLearningFrame/data_type/batch/duplicate.h  
#### Duplicate Batch中的每个矩阵或标量都是相同的
![][image11]

### 总结
* 通过[标签体系](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/data_type/facilities/tags.h)，为每个类型引入一个类别标签（[`DataCategory`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/data_type/facilities/traits.h#L111)可以根据类型返回类别），STL也有标签的概念，为不同的迭代器赋予不同的标签（如双向迭代器、随机访问迭代器等） 
* 使用模板参数而非函数参数来传递标签信息的好处是不需要提供标签的定义  
* MDL中的矩阵是浅拷贝的 
  
## 3 运算与表达式模板
MDL中运算的设计体现了两个原则
- 复用性，将不同运算中相似的逻辑提取出来  
- 优化，使用表达式模板（运算模板）  

MDL中的运算操作本质上是构造表达式模板实例，真正的求值计算是延后的  

![][image13]  
OperCateCal用来得到计算结果的类别
OperOrganzier类提供了运算结果的尺寸接口

## 4 求值与优化  
求值逻辑：MDL/MetaDeepLearningFrame/evaluate  
不同的数据类型和运算都需要实现各自的求值逻辑  
### 4.1 求值系统类图
![][image14]
### 4.2 求值核心逻辑EvalPlan中的数据成员
![][image15]
为什么设计[`EvalLayer`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/evaluate/facilities/eval_plan.h#L117)?  
大部分求值请求是在调用EvalPlan::Eval()之前注册到EvalPlan中的，也有一些
请求是在EvalPlan::Eval()执行过程中产生的，为了处理这些请求，设计EvalLayer。

为什么设计[`Vector<EvalCluster>`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/evaluate/facilities/eval_plan.h#L45)?  
为了刻画求值次序  

为什么设计[`EvalCluster`](https://github.com/1274085042/MDL/blob/main/MetaDeepLearningFrame/evaluate/facilities/eval_plan.h#L38)?  

**EvalGroup负责存放同类计算，这些同类计算可以使用第三方库的批量计算接口进行加速，以点乘为例，Intel的MathKernelLibrary（MKL）提供了gemm_batch接口，可以一次读入多个矩阵完成矩阵乘法。Nvidia的CUDA也提供了类似的功能。**  

**EvalCluster的形式是map，这是为了方便不同种类的计算可以采用多线程的形式进行加速。**



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
[image11]: ./Explanation/Duplicate.png  
[image12]: ./Explanation/Scalar_Batch.png
[image13]: ./Explanation/Operator.png
[image14]: ./Explanation/Evaluate.png
[image15]: ./Explanation/EvalPlan.png
