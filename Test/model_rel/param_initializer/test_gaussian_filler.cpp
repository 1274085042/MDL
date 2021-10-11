#include "../../../MetaDeepLearningFrame/model_rel/param_initializer/gaussian_filler.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace MDL;

void TestGaussianFiller()
{
    cout<<"Start test gaussian filler ..."<<endl;
    GaussianFiller<> gf(1.5,3.3);  
    Matrix<float, DeviceTags::CPU> m1(3000,3000);
    gf.Fill(m1);

    float mean=0;
    float var=0;

    for (int i = 0; i < m1.RowNum(); ++i)
    {
        for(int j=0;j<m1.ColNum();++j)
        {
            mean+=m1(i,j);
        }
    }
    mean/=(m1.RowNum()*m1.ColNum());

    for (int i = 0; i < m1.RowNum(); ++i)
    {
        for(int j=0;j<m1.ColNum();++j)
        {
            var+=((m1(i,j)-mean)*(m1(i,j)-mean));
        }
    }

    var/=(m1.RowNum()*m1.ColNum());

    cout<<"mean delta: "<< (mean-1.5)<<"     std delta: "<<(sqrt(var)-3.3)<<endl;
    cout<<"done"<<endl;
}

int main()
{
    TestGaussianFiller();
}