#pragma once
#include "cbaseanimating.h"

class IHasLocalToGlobalFlexSettings
{
public:
	virtual void EnsureTranslations(const /*flexsettinghdr_t*/ void* pSettinghdr) = 0;
};

class CBaseFlex : public CBaseAnimatingOverlay, public IHasLocalToGlobalFlexSettings
{
public:
	NETVAR(m_flexWeight, "CBaseFlex->m_flexWeight", void*)
	NETVAR(m_blinktoggle, "CBaseFlex->m_blinktoggle", int)
	NETVAR(m_viewtarget, "CBaseFlex->m_viewtarget", Vec3)
};