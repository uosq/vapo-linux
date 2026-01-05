#pragma once

#include "../../vtables.h"

class CTFPlayer;

class MoveHelper
{
public:
	void SetHost(CTFPlayer* player)
	{
		vtable::call<12, void>(this);
	}
};