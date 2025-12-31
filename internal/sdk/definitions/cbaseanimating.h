#pragma once

#include "../classes/entity.h"
#include "../netvars/netvar.h"
#include "cbasehandle.h"

class CBaseAnimating : public CBaseEntity
{
public:
	NETVAR(m_nSequence, "CBaseAnimating->m_nSequence", int)
	NETVAR(m_nForceBone, "CBaseAnimating->m_nForceBone", int)
	NETVAR(m_vecForce, "CBaseAnimating->m_vecForce", Vec3)
	NETVAR(m_nSkin, "CBaseAnimating->m_nSkin", int)
	NETVAR(m_nBody, "CBaseAnimating->m_nBody", int)
	NETVAR(m_nHitboxSet, "CBaseAnimating->m_nHitboxSet", int)
	NETVAR(m_flModelScale, "CBaseAnimating->m_flModelScale", float)
	NETVAR(m_flModelWidthScale, "CBaseAnimating->m_flModelWidthScale", float)
	NETVAR(m_flPlaybackRate, "CBaseAnimating->m_flPlaybackRate", float)
	NETVAR(m_flEncodedController, "CBaseAnimating->m_flEncodedController", void*)
	NETVAR(m_bClientSideAnimation, "CBaseAnimating->m_bClientSideAnimation", bool)
	NETVAR(m_bClientSideFrameReset, "CBaseAnimating->m_bClientSideFrameReset", bool)
	NETVAR(m_nNewSequenceParity, "CBaseAnimating->m_nNewSequenceParity", int)
	NETVAR(m_nResetEventsParity, "CBaseAnimating->m_nResetEventsParity", int)
	NETVAR(m_nMuzzleFlashParity, "CBaseAnimating->m_nMuzzleFlashParity", int)
	NETVAR(m_hLightingOrigin, "CBaseAnimating->m_hLightingOrigin", CBaseHandle)
	NETVAR(m_hLightingOriginRelative, "CBaseAnimating->m_hLightingOriginRelative", CBaseHandle)
	NETVAR(m_flCycle, "CBaseAnimating->m_flCycle", float)
	NETVAR(m_fadeMinDist, "CBaseAnimating->m_fadeMinDist", float)
	NETVAR(m_fadeMaxDist, "CBaseAnimating->m_fadeMaxDist", float)
	NETVAR(m_flFadeScale, "CBaseAnimating->m_flFadeScale", float)
	inline std::array<float, 24>& m_flPoseParameter()
	{
		static int nOffset = netvars[fnv::Hash("CBaseAnimating->m_flPoseParameter")];
		return *reinterpret_cast<std::array<float, 24>*>(uintptr_t(this) + nOffset);
	}
};

class CBaseAnimatingOverlay : public CBaseAnimating
{
public:

};

class CCurrencyPack : public CBaseAnimating
{
public:
	NETVAR(m_bDistributed, "CCurrencyPack->m_bDistributed", bool)
};

class CHalloweenPickup : public CBaseAnimating
{
public:
};

class CHalloweenGiftPickup : public CHalloweenPickup
{
public:
	NETVAR(m_hTargetPlayer, "CHalloweenGiftPickup->m_hTargetPlayer", CBaseHandle)
};