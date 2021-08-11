#pragma once

namespace MDL
{
    template<typename tOpTag, typename tOp1, typename ...tOpn>
    struct OperElementType_
    {
        using type = typename tOp1::ElementType;
    };
    
    template<typename tOpTag, typename tOp1, typename ...tOpn>
    struct OperDeviceType_
    {
        using type = typename tOp1::DeviceType;
    };  
}