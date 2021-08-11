#include "../../MetaDeepLearningFrame/operators/facilities/category_cal.h"
#include "../../MetaDeepLearningFrame/operators/facilities/tags.h"
#include "../../MetaDeepLearningFrame/data_type/matrices/matrices.h"
#include "../../MetaDeepLearningFrame/data_type/matrices/trival_matrix.h"
#include "../../MetaDeepLearningFrame/data_type/scalar/scalar.h"
#include <iostream>

using namespace MDL;
using namespace std;

void test()
{
    static_assert(std::is_same<OperCateCal<BinaryOpTags::Add, TrivalMatrix<int, DeviceTags::CPU, Scalar<int, DeviceTags::CPU>>, Matrix<float, DeviceTags::CPU>>,CategoryTags::Matrix>::value,"Error!");
}

int main()
{
    test();
    return 0;
}
