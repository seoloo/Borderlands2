#pragma once
#include "Engine_Macro.h"

BEGIN(Engine)

template <typename T>
class ENGINE_DLL CFactory final
{
public:
    static T* Create(Args&&... args)
    {
        return T::Create(args...);
    }
};

END
