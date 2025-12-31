#pragma once

#include <string>
#include <string.h>
#include <cstdint>
#include "../definitions/types.h"
#include "../interfaces/interfaces.h"
#include "../definitions/icliententity.h"
#include "../netvars/netvar.h"

#define MULTIPLAYER_BACKUP 90

typedef CHandle<CBaseEntity> EHANDLE;

class CBaseEntity : public IClientEntity
{
public:
	NETVAR(m_flAnimTime, "CBaseEntity->m_flAnimTime", float);
	NETVAR(m_flSimulationTime, "CBaseEntity->m_flSimulationTime", float);
	NETVAR(m_ubInterpolationFrame, "CBaseEntity->m_ubInterpolationFrame", int);
	NETVAR(m_vecOrigin, "CBaseEntity->m_vecOrigin", Vec3);
	NETVAR(m_angRotation, "CBaseEntity->m_angRotation", Vec3);
	NETVAR(m_nModelIndex, "CBaseEntity->m_nModelIndex", int);
	NETVAR(m_fEffects, "CBaseEntity->m_fEffects", int);
	NETVAR(m_nRenderMode, "CBaseEntity->m_nRenderMode", int);
	NETVAR(m_nRenderFX, "CBaseEntity->m_nRenderFX", int);
	NETVAR(m_clrRender, "CBaseEntity->m_clrRender", Color);
	NETVAR(m_iTeamNum, "CBaseEntity->m_iTeamNum", int);
	NETVAR(m_CollisionGroup, "CBaseEntity->m_CollisionGroup", int);
	NETVAR(m_flGravity, "CBaseEntity->m_flGravity", float);
	NETVAR(m_flElasticity, "CBaseEntity->m_flElasticity", float);
	NETVAR(m_flShadowCastDistance, "CBaseEntity->m_flShadowCastDistance", float);
	NETVAR(m_hOwnerEntity, "CBaseEntity->m_hOwnerEntity", EHANDLE);
	NETVAR(m_hEffectEntity, "CBaseEntity->m_hEffectEntity", EHANDLE);
	NETVAR(moveparent, "CBaseEntity->moveparent", int);
	NETVAR(m_iParentAttachment, "CBaseEntity->m_iParentAttachment", int);
	NETVAR(m_Collision, "CBaseEntity->m_Collision", void*);
	NETVAR(m_vecMinsPreScaled, "CBaseEntity->m_vecMinsPreScaled", Vec3);
	NETVAR(m_vecMaxsPreScaled, "CBaseEntity->m_vecMaxsPreScaled", Vec3);
	NETVAR(m_vecMins, "CBaseEntity->m_vecMins", Vec3);
	NETVAR(m_vecMaxs, "CBaseEntity->m_vecMaxs", Vec3);
	NETVAR(m_nSolidType, "CBaseEntity->m_nSolidType", int);
	NETVAR(m_usSolidFlags, "CBaseEntity->m_usSolidFlags", int);
	NETVAR(m_nSurroundType, "CBaseEntity->m_nSurroundType", int);
	NETVAR(m_triggerBloat, "CBaseEntity->m_triggerBloat", int);
	NETVAR(m_bUniformTriggerBloat, "CBaseEntity->m_bUniformTriggerBloat", bool);
	NETVAR(m_vecSpecifiedSurroundingMinsPreScaled, "CBaseEntity->m_vecSpecifiedSurroundingMinsPreScaled", Vec3);
	NETVAR(m_vecSpecifiedSurroundingMaxsPreScaled, "CBaseEntity->m_vecSpecifiedSurroundingMaxsPreScaled", Vec3);
	NETVAR(m_vecSpecifiedSurroundingMins, "CBaseEntity->m_vecSpecifiedSurroundingMins", Vec3);
	NETVAR(m_vecSpecifiedSurroundingMaxs, "CBaseEntity->m_vecSpecifiedSurroundingMaxs", Vec3);
	NETVAR(m_iTextureFrameIndex, "CBaseEntity->m_iTextureFrameIndex", int);
	NETVAR(m_PredictableID, "CBaseEntity->m_PredictableID", int);
	NETVAR(m_bIsPlayerSimulated, "CBaseEntity->m_bIsPlayerSimulated", bool);
	NETVAR(m_bSimulatedEveryTick, "CBaseEntity->m_bSimulatedEveryTick", bool);
	NETVAR(m_bAnimatedEveryTick, "CBaseEntity->m_bAnimatedEveryTick", bool);
	NETVAR(m_bAlternateSorting, "CBaseEntity->m_bAlternateSorting", bool);
	NETVAR(m_nModelIndexOverrides, "CBaseEntity->m_nModelIndexOverrides", void*);
	NETVAR(movetype, "CBaseEntity->movetype", int);

	bool IsPlayer()
	{
		return GetClassID() == ETFClassID::CTFPlayer;
	}

	bool IsWeapon()
	{
		return true;
	}

	int GetIndex()
	{
		return GetClientNetworkable()->entindex();
	}
};