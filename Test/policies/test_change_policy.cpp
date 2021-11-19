#include "../../MetaDeepLearningFrame/policies/change_policy.h"
#include "../../MetaDeepLearningFrame/layers/facilities/policies.h"
#include <iostream>

using namespace MDL;

struct Layer1;
struct Layer2;
struct Layer3;

void test_change_policy()
{
    std::cout<<"Test change policy...\n";
    using input = PolicyContainer<PBatchMode, SubPolicyContainer<Layer1, PUpdate>, PNoUpdate>;

    using check1 = ChangePolicy<PNoBatchMode, input>;
    static_assert(std::is_same<check1, PolicyContainer<SubPolicyContainer<Layer1, PUpdate>, PNoUpdate, PNoBatchMode>>::value,"Check Error");

    using check2 = ChangePolicy<PFeedbackOutput, input>;
    static_assert(std::is_same<check2, PolicyContainer<PBatchMode, SubPolicyContainer<Layer1, PUpdate>, PNoUpdate, PFeedbackOutput>>::value,"Check Error");

    std::cout<<"done"<<std::endl;
}

int main()
{
    test_change_policy();
}