#pragma once

#include "../../../sdk/definitions/cusercmd.h"
#include "../../../sdk/classes/player.h"
#include "../../../sdk/classes/weaponbase.h"
#include "../../../sdk/helpers/helper.h"
#include "../../../sdk/handle_utils.h"
#include "../../../sdk/definitions/ienginetrace.h"
#include "../../../sdk/definitions/cgametrace.h"
#include "../../../sdk/definitions/ctracefilters.h"
#include "../../../sdk/definitions/bspflags.h"
#include "../../../settings.h"
#include "../../../sdk/definitions/studio.h"

#include "../utils/utils.h"
#include "../../entitylist/entitylist.h"
#include <climits>

struct AimbotMelee
{
	inline void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, AimbotState& state)
	{
		if (settings.aimbot.melee == MeleeMode::NONE || pWeapon->GetWeaponID() == TF_WEAPON_KNIFE)
			return;
		
		std::vector<PotentialTarget> targets;

		bool is_a_sword = static_cast<int>(AttributeHookValue(0, "is_a_sword", pWeapon, nullptr, true));
		float range = (is_a_sword ? 72.0f : 48.0f) * 1.9f;
		int localTeam = pLocal->m_iTeamNum();

		Vector shootPos = pLocal->GetEyePos();
		
		CGameTrace trace;
		CTraceFilterHitscan filter;
		filter.pSkip = pLocal;
		
		Vector swingMins, swingMaxs;
		float fBoundsScale = AttributeHookValue(1.0f, "melee_bounds_multiplier", pWeapon, nullptr, true);
		swingMins.Set(-18 * fBoundsScale, -18 * fBoundsScale, -18 * fBoundsScale);
		swingMaxs.Set(18 * fBoundsScale, 18 * fBoundsScale, 18 * fBoundsScale);

		Vector targetAngle;
		CBaseEntity* target = nullptr;
		float highestDot = -1.0f;

		MeleeMode mode = settings.aimbot.melee;
		float fovRad = DEG2RAD(mode == MeleeMode::LEGIT ? 90.0f : 180.0f);
		float minDot = cosf(fovRad);

		Vector viewAngles, viewForward;
		interfaces::Engine->GetViewAngles(viewAngles);
		Math::AngleVectors(viewAngles, &viewForward);
		viewForward.Normalize();

		auto scanList = [&](const auto& list)
		{
			for (CBaseEntity* enemy : list)
			{
				if (!AimbotUtils::IsValidEntity(enemy, 0))
					continue;

				Vector hitPos = enemy->GetCenter();
				Vector dir = hitPos - shootPos;
				float distance = dir.Normalize();
				if (distance > range)
					continue;

				float dot = dir.Dot(viewForward);
				if (dot < minDot || highestDot > dot)
					continue;

				helper::engine::TraceHull(shootPos, shootPos + (dir * range), swingMins, swingMaxs, MASK_SHOT_HULL, &filter, &trace);

				if (!trace.DidHit() || trace.m_pEnt != enemy)
					continue;

				targetAngle = dir.ToAngle();
				target = enemy;
				highestDot = dot;
			}
		};

		{
			bool bCanHitTeammates = pWeapon->CanHitTeammates();
			TeamMode teamMode = settings.aimbot.teamMode;

			if (!bCanHitTeammates || teamMode == TeamMode::ONLYENEMY || teamMode == TeamMode::BOTH)
				scanList(EntityList::GetEnemies());

			if (bCanHitTeammates && (teamMode == TeamMode::ONLYTEAMMATE || teamMode == TeamMode::BOTH))
				scanList(EntityList::GetTeammates());
		}

		if (target == nullptr)
			return;

		if (settings.aimbot.autoshoot)
			pCmd->buttons |= IN_ATTACK;

		if (helper::localplayer::IsAttacking(pLocal, pWeapon, pCmd))
		{
			Vector angle = targetAngle;
			state.angle = angle;
			pCmd->viewangles = angle;
			state.shouldSilent = true;
		}

		EntityList::m_pAimbotTarget = target;
		state.running = true;
	}
};

inline std::string GetMeleeModeName()
{
	switch(settings.aimbot.melee)
	{
		case MeleeMode::NONE: return "None";
		case MeleeMode::LEGIT: return "Legit";
		case MeleeMode::RAGE: return "Rage";
		default: return "Invalid";
        }
}