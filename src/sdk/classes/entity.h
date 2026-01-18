#pragma once

#include <string>
#include <string.h>
#include <cstdint>
#include "../definitions/types.h"
#include "../interfaces/interfaces.h"
#include "../definitions/icliententity.h"
#include "../netvars/netvar.h"
#include "../handle_utils.h"

#define MULTIPLAYER_BACKUP 90

// entity + 0xd4 = m_iHealth

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
	NETVAR(m_flNextAttack, "CBaseCombatCharacter->m_flNextAttack", float)

	bool IsPlayer()
	{
		return GetClassID() == ETFClassID::CTFPlayer;
	}

	bool IsWeapon()
	{
		// I think I got every one of them
		switch(GetClassID())
		{
			case ETFClassID::CTFWeaponInvis:
			case ETFClassID::CTFWeaponPDA:
			case ETFClassID::CWeaponMedigun:
			case ETFClassID::CTFWeaponPDA_Spy: // wtf is this?
			case ETFClassID::CTFWeaponBuilder:
			case ETFClassID::CTFWeaponSapper:
			case ETFClassID::CTFMinigun:
			case ETFClassID::CTFSniperRifleClassic:
			case ETFClassID::CTFSniperRifle:
			case ETFClassID::CTFSniperRifleDecap:
			case ETFClassID::CTFBaseProjectile:
			case ETFClassID::CTFBat:
			case ETFClassID::CTFBat_Giftwrap:
			case ETFClassID::CTFBat_Fish:
			case ETFClassID::CTFBat_Wood:
			case ETFClassID::CTFBonesaw:
			case ETFClassID::CTFBottle:
			case ETFClassID::CTFBuffBanner:
			case ETFClassID::CTFCannon:
			case ETFClassID::CTFSMG:
			case ETFClassID::CTFChargedSMG:
			case ETFClassID::CTFCleaver:
			case ETFClassID::CTFClub:
			case ETFClassID::CTFCompoundBow:
			case ETFClassID::CTFCrossbow:
			case ETFClassID::CTFDRGPomson:
			case ETFClassID::CTFFireAxe:
			case ETFClassID::CTFFists:
			case ETFClassID::CTFFlameRocket:
			case ETFClassID::CTFFlameThrower:
			case ETFClassID::CTFWeaponFlameBall:
			case ETFClassID::CTFFlareGun:
			case ETFClassID::CTFFlareGun_Revenge:
			case ETFClassID::CTFGrenadeLauncher:
			case ETFClassID::CTFJar:
			case ETFClassID::CTFJarGas:
			case ETFClassID::CTFJarMilk:
			case ETFClassID::CTFKatana:
			case ETFClassID::CTFKnife:
			case ETFClassID::CTFLaserPointer:
			case ETFClassID::CTFLunchBox_Drink:
			case ETFClassID::CTFMechanicalArm:
			case ETFClassID::CTFParachute:
			case ETFClassID::CTFParachute_Primary:
			case ETFClassID::CTFParachute_Secondary:
			case ETFClassID::CTFPistol:
			case ETFClassID::CTFPistol_Scout:
			case ETFClassID::CTFPistol_ScoutPrimary:
			case ETFClassID::CTFPistol_ScoutSecondary:
			case ETFClassID::CTFPipebombLauncher:
			case ETFClassID::CTFPEPBrawlerBlaster:
			case ETFClassID::CTFRocketLauncher:
			case ETFClassID::CTFRocketLauncher_AirStrike:
			case ETFClassID::CTFRocketLauncher_DirectHit:
			case ETFClassID::CTFRocketLauncher_Mortar:
			case ETFClassID::CTFRocketPack:
			case ETFClassID::CTFShotgun:
			case ETFClassID::CTFShotgun_HWG:
			case ETFClassID::CTFShotgun_Pyro:
			case ETFClassID::CTFShotgun_Revenge:
			case ETFClassID::CTFShotgunBuildingRescue:
			case ETFClassID::CTFShotgun_Soldier:
			case ETFClassID::CTFRevolver:
			return true;

			default: return false;
		}
		return false;
	}

	int GetIndex()
	{
		return GetClientNetworkable()->entindex();
	}

	bool IsSentry()
	{
		return GetClassID() == ETFClassID::CObjectSentrygun;
	}

	bool IsDispenser()
	{
		return GetClassID() == ETFClassID::CObjectDispenser;
	}

	bool IsTeleporter()
	{
		return GetClassID() == ETFClassID::CObjectTeleporter;
	}

	bool IsBuilding()
	{
		switch(GetClassID())
		{
			case ETFClassID::CObjectDispenser:
			case ETFClassID::CObjectSentrygun:
			case ETFClassID::CObjectTeleporter:
				return true;
			default: break;
		}

		return false;
	}

	bool IsRobot()
	{
		return GetClassID() == ETFClassID::CTFRobotDestruction_Robot;
	}

	CBaseEntity* GetMoveParent()
	{
		static int offset = netvars[fnv::HashConst("CBaseEntity->moveparent")] - 8;
		auto m_moveparent = *reinterpret_cast<EHANDLE*>(uintptr_t(this) + offset);
		return m_moveparent ? HandleAs<CBaseEntity*>(m_moveparent) : nullptr;
	}

	inline CBaseEntity* NextMovePeer()
	{
		static int nOffset = netvars[fnv::HashConst("CBaseEntity->moveparent")] - 16;
		auto m_pMovePeer = *reinterpret_cast<EHANDLE*>(uintptr_t(this) + nOffset);
		return m_pMovePeer ? HandleAs<CBaseEntity*>(m_pMovePeer) : nullptr;
	}

	inline CBaseEntity* FirstMoveChild()
	{
		static int nOffset = netvars[fnv::HashConst("CBaseEntity->moveparent")] - 24;
		auto m_pMoveChild = *reinterpret_cast<EHANDLE*>(uintptr_t(this) + nOffset);
		return m_pMoveChild ? HandleAs<CBaseEntity*>(m_pMoveChild) : nullptr;
	}

	inline CBaseEntity* const GetRootMoveParent()
	{
		CBaseEntity* ent = this;
		CBaseEntity* parent = GetMoveParent();

		while (parent)
		{
			ent = parent;
			parent = ent->GetMoveParent();
		}

		return ent;
	}
};