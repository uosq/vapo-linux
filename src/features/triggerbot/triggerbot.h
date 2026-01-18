#pragma once

#include "../../settings.h"
#include "../../sdk/classes/player.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/definitions/cgametrace.h"
#include "../../sdk/definitions/ctracefilters.h"
#include "../aimbot/utils/utils.h"

namespace Triggerbot
{
	static bool IsBehindAndFacingEntity(CTFPlayer *pLocal, CTFPlayer* pTarget)
	{
		Vector dir = (pTarget->GetCenter() - pLocal->GetCenter());
		dir.z = 0;
		dir.Normalize();

		Vector localForward;
		Math::AngleVectors(pLocal->m_angEyeAngles(), &localForward);
		localForward.z = 0;
		localForward.Normalize();

		Vector targetForward;
		Math::AngleVectors(pTarget->m_angEyeAngles(), &targetForward);
		targetForward.z = 0;
		targetForward.Normalize();

		float posVsTargetView = dir.Dot(targetForward);
		float posVsLocalView = dir.Dot(localForward);
		float viewAnglesDot = localForward.Dot(targetForward);

		bool isBehind = posVsTargetView > 0.0f; // for some reason this is positive, but in the tf2's source code it is negative wtf
		bool isLookingAtTarget = posVsLocalView > 0.5f;
		bool isFacingBack = viewAnglesDot > -0.3f;

		return isBehind && isLookingAtTarget && isFacingBack;
	}

	static bool CanBackstabEntity(CTFPlayer* pLocal, CTFPlayer* pTarget)
	{
		if (pLocal == nullptr || pTarget == nullptr)
			return false;

		int iNoBackstab = AttributeHookValue(1.0, "cannot_be_backstabbed", pTarget, nullptr, true);
		if (iNoBackstab == 0)
			return false;

		if (IsBehindAndFacingEntity(pLocal, pTarget))
			return true;

		return false;
	}

	static void AutoBackstab(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
	{
		if (pLocal == nullptr || pWeapon == nullptr)
			return;

		if (!settings.triggerbot.autobackstab)
			return;

		if (pWeapon->GetWeaponID() != TF_WEAPON_KNIFE)
			return;

		CGameTrace trace;
		CTraceFilterHitscan filter;
		filter.pSkip = pLocal;

		Vector viewAngles, forward;
		interfaces::Engine->GetViewAngles(viewAngles);
		Math::AngleVectors(viewAngles, &forward);

		Vector start = pLocal->GetAbsOrigin() + pLocal->m_vecViewOffset();
		Vector end = start + (forward * 48);

		int localTeam = pLocal->m_iTeamNum();

		helper::engine::Trace(start, end, MASK_SHOT_HULL, &filter, &trace);

		if (!trace.DidHit() || trace.m_pEnt == nullptr)
			return;

		if (!AimbotUtils::IsValidEntity(trace.m_pEnt, localTeam))
			return;

		if (!CanBackstabEntity(pLocal, static_cast<CTFPlayer*>(trace.m_pEnt)))
			return;

		pCmd->buttons |= IN_ATTACK;
	}

	static void Hitscan(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
	{
		if (pLocal == nullptr || pWeapon == nullptr)
			return;

		if (!settings.triggerbot.hitscan)
			return;

		CGameTrace trace;
		CTraceFilterHitscan filter;
		filter.pSkip = pLocal;

		Vector viewAngles, forward;
		interfaces::Engine->GetViewAngles(viewAngles);
		Math::AngleVectors(viewAngles, &forward);

		Vector start = pLocal->GetAbsOrigin() + pLocal->m_vecViewOffset();
		Vector end = start + (forward * 2048);

		int localTeam = pLocal->m_iTeamNum();

		helper::engine::Trace(start, end, MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);

		if (!trace.DidHit() || trace.m_pEnt == nullptr)
			return;

		if (!AimbotUtils::IsValidEntity(trace.m_pEnt, localTeam))
			return;

		if (!helper::localplayer::CanShoot(pLocal, pWeapon))
			return;

		pCmd->buttons |= IN_ATTACK;
	}

	static void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
	{
		if (pLocal == nullptr || pWeapon == nullptr || pCmd == nullptr)
			return;

		if (!settings.triggerbot.enabled)
			return;

		ButtonCode_t key = interfaces::InputSystem->StringToButtonCode(settings.triggerbot.key.c_str());
		if (key != BUTTON_CODE_INVALID && !interfaces::InputSystem->IsButtonDown(key))
			return;

		if (settings.triggerbot.hitscan && pWeapon->IsHitscan())
			Hitscan(pLocal, pWeapon, pCmd);

		if (settings.triggerbot.autobackstab && pWeapon->IsMelee())
			AutoBackstab(pLocal, pWeapon, pCmd);
	}
}