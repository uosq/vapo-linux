#pragma once
#include "definitions/cbasehandle.h"
#include "interfaces/interfaces.h"

/*
I would like to say how much I hate c++
but then I would waste a month talking non stop about the fucking header system
fuck you whoever thought it was a good idea
*/

//Casts to T
template <typename T>
inline T* HandleAs(const CBaseHandle& h)
{
    return static_cast<T*>(
        interfaces::EntityList->GetClientEntity(h.GetEntryIndex())
    );
}
