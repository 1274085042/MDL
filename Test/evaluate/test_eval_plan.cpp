
#include "../../MetaDeepLearningFrame/data_type/facilities/tags.h"
#include "../../MetaDeepLearningFrame/data_type/matrices/one_hot_vector.h"

#include <iostream>
using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = MDL::DeviceTags::CPU;

void TestEvalPlan1()
{
    cout<<"Test eval_plan case 1..."<<endl;
    auto ohv = OneHotVector<int, CheckDevice>(100,37);

    auto er = ohv.EvalRegister();

    try
    {
        er.Data();
        assert(false);
    }
    catch(std::runtime_error&)
    {
    }
    cout<<"done."<<endl;
}

int main()
{
    TestEvalPlan1();
}