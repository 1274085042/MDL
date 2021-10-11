#pragma once

#include "policy_container.h"

namespace MDL
{
    template <template <typename tPolicyContainer> class T, typename ...policies>
    using InjectPolicy = T<PolicyContainer<policies...>>;
}