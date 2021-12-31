
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

void TestEvalPlan2()
{
    cout<<"Test eval_plan case 2..."<<endl;
    auto ohv = OneHotVector<int, CheckDevice> (100, 37);

    auto er1 = ohv.EvalRegister();
    EvalPlan<CheckDevice>::Eval();
    auto er2 = ohv.EvalRegister();
    assert(er1.Data() == er2.Data());
    cout<<"done."<<endl;
}

void TestEvalPlan3()
{
    cout<<"Test eval_plan case 3..."<<endl;
    auto ohv1 = OneHotVector<int, CheckDevice>(100, 37);
    auto ohv2 = ohv1;
    auto er1 = ohv1.EvalRegister();
    EvalPlan<CheckDevice>::Eval();
    auto er2 = ohv1.EvalRegister();
    assert(er1.Data() == er2.Data());
    cout<<"done."<<endl;
}

void TestEvalPlan4()
{
    cout<<"Test eval_plan case 4..."<<endl;
    auto ohv = OneHotVector<int, CheckDevice>(100, 37);

    auto rm1 = ohv.EvalRegister();
    auto rm2 = ohv.EvalRegister();
    EvalPlan<CheckDevice>::Eval();
    assert(rm1.Data() == rm2.Data());

    cout<<"done."<<endl;
}

void TestEvalPlan5()
{
    cout<<"Test eval_plan case 5..."<<endl;
    auto ohv1 = OneHotVector<int, CheckDevice>(100, 37);
    auto ohv2 = ohv1;
    auto rm1 = ohv1.EvalRegister();
    auto rm2 = ohv2.EvalRegister();
    EvalPlan<CheckDevice>::Eval();
    assert(rm1.Data() == rm2.Data());

    cout<<"done."<<endl;
}

int main()
{
    TestEvalPlan1();
    TestEvalPlan2();
    TestEvalPlan3();
    TestEvalPlan4();
    TestEvalPlan5();
}