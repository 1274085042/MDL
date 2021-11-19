#include "../../MetaDeepLearningFrame/layers/facilities/policies.h"
#include "../../MetaDeepLearningFrame/policies/policy_operations.h"
#include <iostream>

using namespace MDL;

struct Layer1;
struct Layer2;
struct Layer3;


void test_policy_operations()
{
   
    std::cout<<"Test policy operations ...\n";
    using input = PolicyContainer<PBatchMode, SubPolicyContainer<Layer1, PNoBatchMode, SubPolicyContainer<Layer2>>>;

    using check1 = SubPolicyPicker<input, Layer3>;
    static_assert(std::is_same<check1, PolicyContainer<PBatchMode>>::value, "Check Error");

    using check2 = SubPolicyPicker<input, Layer1>;
    static_assert(std::is_same<check2, PolicyContainer<PNoBatchMode,  SubPolicyContainer<Layer2>>>::value, "Check Error");

    using check3 = SubPolicyPicker<check2, Layer3>;
    static_assert(std::is_same<check3, PolicyContainer<PNoBatchMode>>::value, "Check Error");

    using check4 = SubPolicyPicker<check2, Layer2>;
    static_assert(std::is_same<check4, PolicyContainer<PNoBatchMode>>::value, "Check Error");
    
    using Policy3 = PolicyContainer<PBatchMode, SubPolicyContainer<Layer2, PUpdate>>;
    using check5 = SubPolicyPicker<Policy3, Layer2>;

    //static_assert(std::is_same<check5, PolicyContainer<PUpdate, PBatchMode>>::value, "Check Error");

    std::cout<<"done"<<std::endl;
}

void test_OrValue()
{
    using input = PolicyContainer<PBatchMode>;
    constexpr static bool value = OrValue<false, PolicyExist_<PBatchMode, input>>;
    std::cout<<"value: "<< value<<std::endl;
}

int main()
{
    test_policy_operations();
    //test_OrValue();
    return 0;
}
