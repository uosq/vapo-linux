#pragma once

#include "entity.h"

class CTFRobotDestruction_Robot: public CBaseEntity
{
public:
	NETVAR(m_iHealth, "CTFRobotDestruction_Robot->m_iHealth", int);
	NETVAR(m_iMaxHealth, "CTFRobotDestruction_Robot->m_iMaxHealth", int);
};