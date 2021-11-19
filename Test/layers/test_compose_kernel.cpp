#include "../../MetaDeepLearningFrame/layers/compose/compose_kernel.h"
#include "../../MetaDeepLearningFrame/layers/elementary/add_layer.h"
#include "../../MetaDeepLearningFrame/layers/elementary/bias_layer.h"
#include "../../MetaDeepLearningFrame/layers/elementary/element_mul_layer.h"

#include <iostream>

using namespace MDL;
using namespace std;

struct Layer1; 
struct Layer2; 
struct Layer3; 
struct Layer4; 
struct Layer5; 
struct Layer6;

struct Input1; 
struct Input2; 
struct Input3;

struct Output1; 
struct Output2;

void test_compose_kernel_1()
{
    cout << "Test compose kernel SeparateParameters_ ...\n";
    using check1 = SeparateParameters_< Sublayer<Layer1, AddLayer>,
                                        Sublayer<Layer2, BiasLayer>,
                                        Sublayer<Layer3, ElementMulLayer>,
                                        Sublayer<Layer4, AddLayer>,
                                        InConnect<Input1, Layer1, AddLayerIn1>,
                                        InConnect<Input2, Layer1, AddLayerIn2>,
                                        InConnect<Input1, Layer3, ElementMulLayerIn1>,
                                        InternalConnect<Layer1, LayerIO, Layer3, ElementMulLayerIn2>,
                                        InternalConnect<Layer3, LayerIO, Layer2, LayerIO>,
                                        InternalConnect<Layer2, LayerIO, Layer4, AddLayerIn1>,
                                        InternalConnect<Layer3, LayerIO, Layer4, AddLayerIn2>,
                                        OutConnect<Layer4, LayerIO, Output1>
                                      >;
    static_assert(is_same<check1::SublayerContainerRes, SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                                          Sublayer<Layer2, BiasLayer>,
                                                                          Sublayer<Layer3, ElementMulLayer>,
                                                                          Sublayer<Layer4, AddLayer>>>::value,
                                                                          "Check Error");
    static_assert(is_same<check1::InConnectContainerRes, InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                                            InConnect<Input2, Layer1, AddLayerIn2>,
                                                                            InConnect<Input1, Layer3, ElementMulLayerIn1>>>::value,
                                                                          "Check Error");
    static_assert(is_same<check1::OutConnectContainerRes, OutConnectContainer<OutConnect<Layer4, LayerIO, Output1>>>::value,
                                                                          "Check Error");
    static_assert(is_same<check1::InternalConnectContainerRes, InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer3, ElementMulLayerIn2>,
                                                                                        InternalConnect<Layer3, LayerIO, Layer2, LayerIO>,
                                                                                        InternalConnect<Layer2, LayerIO, Layer4, AddLayerIn1>,
                                                                                        InternalConnect<Layer3, LayerIO, Layer4, AddLayerIn2>>>::value,
                                                                                         "Check Error");                                                                    
    cout <<"done"<< endl;
}

void test_compose_kernel_2()
{
    cout << "Test compose kernel statement legality ...\n";

    static_assert(TagExistInLayerComps_<Layer2, Sublayer<Layer1, AddLayer>,
                                                Sublayer<Layer2, ElementMulLayer>,
                                                Sublayer<Layer3, BiasLayer>>::value,
                                                "Check Error");
    
    static_assert(!TagExistInLayerComps_<Layer4, Sublayer<Layer1, AddLayer>,
                                                 Sublayer<Layer2, ElementMulLayer>,
                                                 Sublayer<Layer3, BiasLayer>>::value,
                                                "Check Error");                                            

    // 1 检查层的名称是否重复
    static_assert(SublayerTagCheck<SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                     Sublayer<Layer2, ElementMulLayer>,
                                                     Sublayer<Layer3, BiasLayer>>>::isUniqueTag,
                "Check Error");
    static_assert(!SublayerTagCheck<SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                      Sublayer<Layer2, ElementMulLayer>,
                                                      Sublayer<Layer2, BiasLayer>>>::isUniqueTag,
                "Check Error");  

    using check1 = InternalConnectCheck<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                 InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                 InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                                 InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                 InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>>;
    static_assert(check1::noSelfCycle, "Check Error");
    static_assert(check1::sourceUnique, "Check Error");
    
    // 2 检查InternalConnect中的层是否形成了自环
    // 3 检查输入源是否唯一
    // 3.1 InternalConnect中的输入源是否唯一
    using check2 = InternalConnectCheck<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                 InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                 InternalConnect<Layer2, LayerIO, Layer2, LayerIO>,
                                                                 InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                 InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>>;
    static_assert(!check2::noSelfCycle, "Check Error");
    static_assert(check2::sourceUnique, "Check Error");

    using check3 = InternalConnectCheck<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                 InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                 InternalConnect<Layer5, LayerIO, Layer3, LayerIO>,
                                                                 InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                 InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>>;
    static_assert(check3::noSelfCycle, "Check Error");
    static_assert(!check3::sourceUnique, "Check Error");

    // 3.2 InConnect中的输入源是否唯一
    using check4 = InputConnectCheck<InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                        InConnect<Input2, Layer1, AddLayerIn2>>>;
    static_assert(check4::sourceUnique, "Check Error");

    using check5 = InputConnectCheck<InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                        InConnect<Input2, Layer1, AddLayerIn1>>>;
    static_assert(!check5::sourceUnique, "Check Error");

    using check6 = InputConnectCheck<InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                        InConnect<Input1, Layer1, AddLayerIn2>>>;
    static_assert(check6::sourceUnique, "Check Error");

    // 3.3 OutConnect中的输入源是否唯一
    using check7 = OutputConnectCheck<OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>>;
    static_assert(check7::sourceUnique, "Check Error");

    using check8 = OutputConnectCheck<OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>,
                                                          OutConnect<Layer5, LayerIO, Output2>>>;
    static_assert(check8::sourceUnique, "Check Error");

    using check9 = OutputConnectCheck<OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>,
                                                          OutConnect<Layer3, LayerIO, Output1>>>;
    static_assert(!check9::sourceUnique, "Check Error");

    // 4 InternalConnect、InConnect和OutConnect中出现的层必须有相应的Sublayer
    // 4.1 InternalConnect中出现的层必须有相应的Sublayer
    using check10 = InternalTagInSublayer<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                   InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                   InternalConnect<Layer5, LayerIO, Layer3, LayerIO>,
                                                                   InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                   InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                          SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                            Sublayer<Layer2, ElementMulLayer>,
                                                            Sublayer<Layer3, BiasLayer>,
                                                            Sublayer<Layer4, BiasLayer>,
                                                            Sublayer<Layer5, AddLayer>>>;
    static_assert(check10::value, "Check Error");

    using check11 = InternalTagInSublayer<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                   InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                   InternalConnect<Layer5, LayerIO, Layer3, LayerIO>,
                                                                   InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                   InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                         SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                           Sublayer<Layer2, ElementMulLayer>,
                                                           Sublayer<Layer3, BiasLayer>,
                                                           Sublayer<Layer4, BiasLayer>>>;
    static_assert(!check11::value, "Check Error");

    using check12 = InternalTagInSublayer<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                   InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                   InternalConnect<Layer5, LayerIO, Layer3, LayerIO>>,
                                         SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                           Sublayer<Layer2, ElementMulLayer>,
                                                           Sublayer<Layer3, BiasLayer>,
                                                           Sublayer<Layer4, BiasLayer>,
                                                           Sublayer<Layer5, AddLayer>>>;
    static_assert(check12::value, "Check Error");

    // 4.2 InConnect中出现的层必须有相应的Sublayer
    using check13 = InputTagInSublayer<InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                         InConnect<Input2, Layer1, AddLayerIn2>,
                                                         InConnect<Input1, Layer2, ElementMulLayerIn2>>,
                                      SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                        Sublayer<Layer2, ElementMulLayer>,
                                                        Sublayer<Layer3, BiasLayer>,
                                                        Sublayer<Layer4, BiasLayer>,
                                                        Sublayer<Layer5, AddLayer>>>;
    static_assert(check13::value, "Check Error");
    
    // 4.3 OutConnect中出现的层必须有相应的Sublayer
    using check14 = OutputTagInSublayer<OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>,
                                       SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                         Sublayer<Layer2, ElementMulLayer>,
                                                         Sublayer<Layer3, BiasLayer>,
                                                         Sublayer<Layer4, BiasLayer>,
                                                         Sublayer<Layer5, AddLayer>>>;
    static_assert(check14::value, "Check Error");

    using check15 = OutputTagInSublayer<OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>,
                                       SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                         Sublayer<Layer3, BiasLayer>,
                                                         Sublayer<Layer4, BiasLayer>>>;
    static_assert(!check15::value, "Check Error");

    // 5 Sublayer声明的层必须在InConnect、OutConnect或者InternalConnect中使用
    using check16 = SublayerTagInOtherArray<SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                              Sublayer<Layer2, ElementMulLayer>,
                                                              Sublayer<Layer3, BiasLayer>,
                                                              Sublayer<Layer4, BiasLayer>,
                                                              Sublayer<Layer5, AddLayer>>,
                                            InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                               InConnect<Input2, Layer1, AddLayerIn2>,
                                                               InConnect<Input1, Layer2, ElementMulLayerIn2>>,
                                            OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>,
                                            InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                     InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                     InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                                     InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                     InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>
                                            >;
    static_assert(check16::value, "Check Error");

    using check17 = SublayerTagInOtherArray<SublayerContainer<Sublayer<Layer1, AddLayer>,
                                                              Sublayer<Layer2, ElementMulLayer>,
                                                              Sublayer<Layer3, BiasLayer>,
                                                              Sublayer<Layer4, BiasLayer>,
                                                              Sublayer<Layer5, AddLayer>,
                                                              Sublayer<Layer6, AddLayer>>,
                                            InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                               InConnect<Input2, Layer1, AddLayerIn2>,
                                                               InConnect<Input1, Layer2, ElementMulLayerIn2>>,
                                            OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>,
                                            InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                     InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                     InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                                     InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                     InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>>;
    static_assert(!check17::value, "Check Error");

    
    // 6 IntenalConnect的输出要么作为某个子层的输入，要么作为复合层的输出
    using check18 = InternalInTagUseful<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                 InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                 InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                                 InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                 InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                           OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>>;
    static_assert(check18::value, "Check Error");

    // Error: Layer2 is useless
    using check19 = InternalInTagUseful<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                 InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                 InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                           OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>>;
    static_assert(!check19::value, "Check Error");

    // Error: Layer5 is useless
    using check20 = InternalInTagUseful<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                 InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                                 InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                                 InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                 InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                           OutConnectContainer<>>;
    static_assert(!check20::value, "Check Error");

    using check21 = InternalInTagUseful<InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                                 InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                                 InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                           OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>,
                                                               OutConnect<Layer2, LayerIO, Output2>>>;
    static_assert(check21::value, "Check Error");

    // 7 InConnect的输出要么作为某个子层的输入，要么作为复合层的输出
    using check22 = InputLayerUserful<InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                         InConnect<Input2, Layer1, AddLayerIn2>,
                                                         InConnect<Input1, Layer2, ElementMulLayerIn2>>,
                                    InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                             InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                             InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                             InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                             InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                    OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>>;
    static_assert(check22::value, "Check Error");

    using check23 = InputLayerUserful<InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                         InConnect<Input2, Layer1, AddLayerIn2>,
                                                         InConnect<Input1, Layer5, ElementMulLayerIn2>>,
                                    InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                             InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                             InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                             InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                             InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                    OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>>;
    static_assert(check23::value, "Check Error");

    using check24= InputLayerUserful<InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                        InConnect<Input2, Layer1, AddLayerIn2>,
                                                        InConnect<Input1, Layer5, ElementMulLayerIn2>>,
                                    InternalConnectContainer<InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                             InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                             InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                             InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                    OutConnectContainer<OutConnect<Layer5, LayerIO, Output1>>>;
    static_assert(!check24::value, "Check Error");

    using check25 = InputLayerUserful<InConnectContainer<InConnect<Input1, Layer1, AddLayerIn1>,
                                                         InConnect<Input2, Layer1, AddLayerIn2>,
                                                         InConnect<Input1, Layer5, ElementMulLayerIn2>>,
                                    InternalConnectContainer<InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                             InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                             InternalConnect<Layer2, LayerIO, Layer4, LayerIO>,
                                                             InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                             InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>>,
                                    OutConnectContainer<>>;
    static_assert(!check25::value, "Check Error");

    cout <<"done"<< endl;
}

void test_compose_kernel_3()
{
    cout << "Test compose kernel topological order...\n";

    using InterConnects = InternalConnectContainer<InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                   InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>,
                                                   InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                   InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                   InternalConnect<Layer2, LayerIO, Layer4, LayerIO>>;

    using Tmpcheck1 = TopologicalOrdering_<SublayerContainer<Sublayer<Layer3, BiasLayer>,
                                                             Sublayer<Layer2, ElementMulLayer>,
                                                             Sublayer<Layer1, AddLayer>,
                                                             Sublayer<Layer4, BiasLayer>,
                                                             Sublayer<Layer6, AddLayer>,
                                                             Sublayer<Layer5, AddLayer>>,
                                            InterConnects>;

    using OrderResAfterPreProcess = Tmpcheck1::OrderResAfterPreProcess;
    using UnOrderResAfterPreProcess = Tmpcheck1::UnOrderResAfterPreProcess;
    using comp1 = SublayerContainer<Sublayer<Layer3, BiasLayer>,
                                    Sublayer<Layer2, ElementMulLayer>,
                                    Sublayer<Layer1, AddLayer>,
                                    Sublayer<Layer4, BiasLayer>,
                                    Sublayer<Layer5, AddLayer>>;
    static_assert(std::is_same<UnOrderResAfterPreProcess, comp1>::value, "Check Error!");                    

    using OrderResAfterMainLoop = Tmpcheck1::OrderResAfterMainLoop;
    using UnOrderResAfterMainLoop = Tmpcheck1::UnOrderResAfterMainLoop;


    using check10 = TopologicalOrdering_<SublayerContainer<Sublayer<Layer3, BiasLayer>,
                                                          Sublayer<Layer2, ElementMulLayer>,
                                                          Sublayer<Layer1, AddLayer>,
                                                          Sublayer<Layer4, BiasLayer>,
                                                          Sublayer<Layer6, AddLayer>,
                                                          Sublayer<Layer5, AddLayer>>,
                                            InterConnects>::type;

    using comp10 = SublayerContainer<Sublayer<Layer6, AddLayer>,
                                    Sublayer<Layer1, AddLayer>,
                                    Sublayer<Layer2, ElementMulLayer>,
                                    Sublayer<Layer3, BiasLayer>,
                                    Sublayer<Layer4, BiasLayer>,
                                    Sublayer<Layer5, AddLayer>>;

    static_assert(std::is_same<check10, comp10>::value, "Check Error");

    cout <<"done"<< endl;

}

void test_compose_kernel_4()
{
    cout << "Test compose kernel sublayer specialization ...\n";

    using InterConnects = InternalConnectContainer<InternalConnect<Layer2, LayerIO, Layer3, LayerIO>,
                                                   InternalConnect<Layer4, LayerIO, Layer5, AddLayerIn2>,
                                                   InternalConnect<Layer1, LayerIO, Layer2, ElementMulLayerIn1>,
                                                   InternalConnect<Layer3, LayerIO, Layer5, AddLayerIn1>,
                                                   InternalConnect<Layer2, LayerIO, Layer4, LayerIO>>;
    using check1 = TopologicalOrdering_<SublayerContainer<Sublayer<Layer3, BiasLayer>,
                                                          Sublayer<Layer2, ElementMulLayer>,
                                                          Sublayer<Layer1, AddLayer>,
                                                          Sublayer<Layer4, BiasLayer>,
                                                          Sublayer<Layer6, AddLayer>,
                                                          Sublayer<Layer5, AddLayer>>,
                                            InterConnects>::type;                                            

    using Policy1 = PolicyContainer<PFeedbackOutput>;

    using Specialization1 = SublayerSpecialization<Policy1, check1, InterConnects>::type;
    using SpecializationComp1 = std::tuple<SpecialSublayer<Layer6, AddLayer<PolicyContainer<PFeedbackOutput>>>,
                                          SpecialSublayer<Layer1, AddLayer<PolicyContainer<PFeedbackOutput>>>,
                                          SpecialSublayer<Layer2, ElementMulLayer<PolicyContainer<PFeedbackOutput>>>,
                                          SpecialSublayer<Layer3, BiasLayer<PolicyContainer<PFeedbackOutput>>>,
                                          SpecialSublayer<Layer4, BiasLayer<PolicyContainer<PFeedbackOutput>>>,
                                          SpecialSublayer<Layer5, AddLayer<PolicyContainer<PFeedbackOutput>>>>;
    static_assert(std::is_same<Specialization1, SpecializationComp1>::value, "Check Error");

    using Policy2 = PolicyContainer<PNoBatchMode, SubPolicyContainer<Layer3, PBatchMode>>;
    using Specialization2 = SublayerSpecialization<Policy2, check1, InterConnects>::type;
    using SpecializationComp2 = std::tuple<SpecialSublayer<Layer6, AddLayer<PolicyContainer<PNoBatchMode>>>,
                                          SpecialSublayer<Layer1, AddLayer<PolicyContainer<PNoBatchMode>>>,
                                          SpecialSublayer<Layer2, ElementMulLayer<PolicyContainer<PNoBatchMode>>>,
                                          SpecialSublayer<Layer3, BiasLayer<PolicyContainer<PBatchMode>>>,
                                          SpecialSublayer<Layer4, BiasLayer<PolicyContainer<PNoBatchMode>>>,
                                          SpecialSublayer<Layer5, AddLayer<PolicyContainer<PNoBatchMode>>>
                                         >;
    static_assert(std::is_same<Specialization2, SpecializationComp2>::value, "Check Error");

    using Policy3 = PolicyContainer<PBatchMode, SubPolicyContainer<Layer2, PUpdate>>;

    using Specialization3 = SublayerSpecialization<Policy3, check1, InterConnects>::type;
    using SpecializationComp3 = std::tuple<SpecialSublayer<Layer6, AddLayer<PolicyContainer<PBatchMode>>>,
                                           SpecialSublayer<Layer1, AddLayer<PolicyContainer<PBatchMode>>>,
                                           SpecialSublayer<Layer2, ElementMulLayer<PolicyContainer<PBatchMode, PUpdate>>>,
                                           SpecialSublayer<Layer3, BiasLayer<PolicyContainer<PBatchMode, PFeedbackOutput>>>,
                                           SpecialSublayer<Layer4, BiasLayer<PolicyContainer<PBatchMode, PFeedbackOutput>>>,
                                           SpecialSublayer<Layer5, AddLayer<PolicyContainer<PBatchMode, PFeedbackOutput>>>>;

    static_assert(std::is_same<Specialization3, SpecializationComp3>::value, "Check Error");

    using Policy4 = PolicyContainer<PCPUDevice, SubPolicyContainer<Layer3, PUpdate>>;
    using Specialization4 = SublayerSpecialization<Policy4, check1, InterConnects>::type;
    using SpecializationComp4 = std::tuple<SpecialSublayer<Layer6, AddLayer<PolicyContainer<PCPUDevice>>>,
                                           SpecialSublayer<Layer1, AddLayer<PolicyContainer<PCPUDevice>>>,
                                           SpecialSublayer<Layer2, ElementMulLayer<PolicyContainer<PCPUDevice>>>,
                                           SpecialSublayer<Layer3, BiasLayer<PolicyContainer<PCPUDevice, PUpdate>>>,
                                           SpecialSublayer<Layer4, BiasLayer<PolicyContainer<PCPUDevice>>>,
                                           SpecialSublayer<Layer5, AddLayer<PolicyContainer<PCPUDevice, PFeedbackOutput>>>>;
    static_assert(std::is_same<Specialization4, SpecializationComp4>::value, "Check Error");

    cout <<"done"<< endl;

}

int main()
{
    test_compose_kernel_1();
    test_compose_kernel_2();
    test_compose_kernel_3();
    test_compose_kernel_4();
};