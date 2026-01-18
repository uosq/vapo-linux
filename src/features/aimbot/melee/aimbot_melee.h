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

struct AimbotMelee
{
	inline void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, AimbotState& state)
	{
		if (pWeapon->GetWeaponID() == TF_WEAPON_KNIFE)
			return;

		Vector shootPos = pLocal->GetCenter();
		float range = 48.0f * 2.0f;

		for (const auto& enemy : EntityList::GetEnemies())
		{
			if (enemy->IsDormant())
				continue;

			if (enemy->IsPlayer())
			{
				CTFPlayer* player = static_cast<CTFPlayer*>(enemy);
				if (!player->IsAlive())
					continue;
			}
			else if (enemy->IsBuilding())
			{
				CBaseObject* building = static_cast<CBaseObject*>(enemy);
				if (building->m_iHealth() <= 0)
					continue;
			}

			Vector hitPos = enemy->GetCenter();
			Vector dir = hitPos - shootPos;
			float distance = dir.Normalize();
			if (distance > range)
				continue;

			if (settings.aimbot.autoshoot)
				pCmd->buttons |= IN_ATTACK;

			if (pWeapon->m_flSmackTime() != -1.0f && interfaces::GlobalVars->curtime >= pWeapon->m_flSmackTime())
			{
				Vector angle = dir.ToAngle();
				state.angle = angle;
				pCmd->viewangles = angle;
				state.shouldSilent = true;
				EntityList::m_pAimbotTarget = enemy;
			}

			state.running = true;
			break;
		}
	}
};