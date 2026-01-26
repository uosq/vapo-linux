#include "triggerbot.h"

namespace Triggerbot
{
	bool IsBehindEntity(CTFPlayer* pLocal, CTFPlayer* pTarget)
	{
		Vector dir = (pTarget->GetCenter() - pLocal->GetCenter());
		dir.z = 0;
		dir.Normalize();

		Vector targetForward;
		Math::AngleVectors(pTarget->m_angEyeAngles(), &targetForward);
		targetForward.z = 0;
		targetForward.Normalize();

		float posVsTargetView = dir.Dot(targetForward);
		return posVsTargetView > 0.0f; // for some reason this is positive, but in the tf2's source code it is negative wtf
	}

	bool IsBehindAndFacingEntity(CTFPlayer *pLocal, CTFPlayer* pTarget)
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

	bool CanBackstabEntity(CTFPlayer* pLocal, CTFPlayer* pTarget)
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

	void AutoBackstabLegit(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
	{
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

	void AutoBackstabRage(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, bool* pSendPacket)
	{
		Vector shootPos = pLocal->GetEyePos();

		for (const auto& enemy : EntityList::GetEnemies())
		{
			if (!enemy->IsPlayer() || !AimbotUtils::IsValidEntity(enemy, pLocal->m_iTeamNum()))
				continue;

			CTFPlayer* pTarget = static_cast<CTFPlayer*>(enemy);
			if (pTarget == nullptr)
				continue;

			Vector center = pTarget->GetCenter();
			Vector dir = center - shootPos;
			float distance = dir.Normalize();

			if (distance > 96.0f)
				continue;

			if (IsBehindEntity(pLocal, pTarget))
			{
				pCmd->buttons |= IN_ATTACK;

				if (helper::localplayer::IsAttacking(pLocal, pWeapon, pCmd))
				{
					Vector viewAngles, viewForward;
					interfaces::Engine->GetViewAngles(viewAngles);
					Math::AngleVectors(viewAngles, &viewForward);
					
					Vector angle = dir.ToAngle();
					pCmd->viewangles = angle;
					*pSendPacket = false;
				}
			}
		}
	}

	void AutoBackstab(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, bool* pSendPacket)
	{
		if (pLocal == nullptr || pWeapon == nullptr)
			return;

		if (pWeapon->GetWeaponID() != TF_WEAPON_KNIFE)
			return;

		switch(settings.triggerbot.autobackstab)
		{
			case AutoBackstabMode::NONE:
			break;

			case AutoBackstabMode::LEGIT:
			{
				AutoBackstabLegit(pLocal, pWeapon, pCmd);
				break;
			}

			case AutoBackstabMode::RAGE:
			{
				AutoBackstabRage(pLocal, pWeapon, pCmd, pSendPacket);
				break;
			}

			default:
			break;
                }
        }

	void Hitscan(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
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

		EntityList::m_pAimbotTarget = trace.m_pEnt;
		pCmd->buttons |= IN_ATTACK;
	}

	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, bool* pSendPacket)
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

                if (settings.triggerbot.autobackstab != AutoBackstabMode::NONE && pWeapon->IsMelee())
			AutoBackstab(pLocal, pWeapon, pCmd, pSendPacket);
	}

	std::string GetAutoBackstabModeName()
	{
		switch(settings.triggerbot.autobackstab)
		{
			case AutoBackstabMode::NONE: return "None";
			case AutoBackstabMode::LEGIT: return "Legit";
			case AutoBackstabMode::RAGE: return "Rage";
			default: return "Unknown";
                }
        }
}