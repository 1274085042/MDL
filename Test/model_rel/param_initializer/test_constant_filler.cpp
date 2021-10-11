#include "../../../MetaDeepLearningFrame/model_rel/param_initializer/constant_filler.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace MDL;

void TestConstantFiller()
{
    cout<<"Start test constant filler ..."<<endl;
    ConstantFiller cf1(0);
    Matrix<float, DeviceTags::CPU> m1(11,23);
    cf1.Fill(m1);
    for(int i=0;i<11;++i)
    {
        for(int j=0;j<23;++j)
        {
            assert(fabs(m1(i,j))<0.001);
        }
    }

    ConstantFiller cf2(1.5f);
    cf2.Fill(m1);
    for(int i=0;i<11;++i)
    {
        for(int j=0;j<23;++j)
        {
            assert(fabs(m1(i,j)-1.5)<0.001);
        }
    }

    cout<<"done"<<endl;
}

int main()
{
    TestConstantFiller();
}