#pragma once

#include "../definitions/cbaseanimating.h"
#include "../../vtables.h"

class CEconEntity : public CBaseAnimating
{
public:
	NETVAR(m_iItemDefinitionIndex, "CEconEntity->m_iItemDefinitionIndex", int);
	void UpdateAttachmentModels() { return vtable::call<213, void>(this); }
};