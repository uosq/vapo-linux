#pragma once

#include "../netvars/netvar.h"
#include "entity.h"

class CBaseObject: public IClientEntity
{
public:
	NETVAR(m_iHealth, "CBaseObject->m_iHealth", int);
	NETVAR(m_iMaxHealth, "CBaseObject->m_iMaxHealth", int);
	NETVAR(m_bHasSapper, "CBaseObject->m_bHasSapper", bool);
	NETVAR(m_iObjectType, "CBaseObject->m_iObjectType", int);
	NETVAR(m_bBuilding, "CBaseObject->m_bBuilding", bool);
	NETVAR(m_bPlacing, "CBaseObject->m_bPlacing", bool);
	NETVAR(m_bCarried, "CBaseObject->m_bCarried", bool);
	NETVAR(m_bCarryDeploy, "CBaseObject->m_bCarryDeploy", bool);
	NETVAR(m_bMiniBuilding, "CBaseObject->m_bMiniBuilding", bool);
	NETVAR(m_flPercentageConstructed, "CBaseObject->m_flPercentageConstructed", float);
	NETVAR(m_fObjectFlags, "CBaseObject->m_fObjectFlags", int);
	NETVAR(m_hBuiltOnEntity, "CBaseObject->m_hBuiltOnEntity", EHANDLE);
	NETVAR(m_bDisabled, "CBaseObject->m_bDisabled", bool);
	NETVAR(m_hBuilder, "CBaseObject->m_hBuilder", EHANDLE);
	NETVAR(m_vecBuildMaxs, "CBaseObject->m_vecBuildMaxs", Vector);
	NETVAR(m_vecBuildMins, "CBaseObject->m_vecBuildMins", Vector);
	NETVAR(m_iDesiredBuildRotations, "CBaseObject->m_iDesiredBuildRotations", int);
	NETVAR(m_bServerOverridePlacement, "CBaseObject->m_bServerOverridePlacement", bool);
	NETVAR(m_iUpgradeLevel, "CBaseObject->m_iUpgradeLevel", int);
	NETVAR(m_iUpgradeMetal, "CBaseObject->m_iUpgradeMetal", int);
	NETVAR(m_iUpgradeMetalRequired, "CBaseObject->m_iUpgradeMetalRequired", int);
	NETVAR(m_iHighestUpgradeLevel, "CBaseObject->m_iHighestUpgradeLevel", int);
	NETVAR(m_iObjectMode, "CBaseObject->m_iObjectMode", int);
	NETVAR(m_bDisposableBuilding, "CBaseObject->m_bDisposableBuilding", bool);
	NETVAR(m_bWasMapPlaced, "CBaseObject->m_bWasMapPlaced", bool);
	NETVAR(m_bPlasmaDisable, "CBaseObject->m_bPlasmaDisable", bool);

	std::string GetName()
	{
		return GetClientClass()->networkName;
	}
};