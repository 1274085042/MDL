#pragma once

#include "../../facilities/var_type_dict.h"

namespace MDL
{
    //异类词典作为正向、反向传播时的输入、输出容器
    struct LayerIO: public VarTypeDict<LayerIO>
    {};

    struct CostLayerIn: public VarTypeDict<CostLayerIn, struct CostLayerLabel>
    {};
}