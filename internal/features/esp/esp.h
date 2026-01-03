#pragma once

#include "../../sdk/interfaces/interfaces.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/definitions/eteam.h"
#include "../../settings.h"

namespace ESP
{
	inline Color GetColor(CTFPlayer* player)
	{
		int team = player->m_iTeamNum();

		if (player->GetClassID() == ETFClassID::CTFPlayer)
		{
			switch (team)
			{
				case ETeam::TEAM_RED:
					return (Color){255, 0, 0, 255};
				case ETeam::TEAM_BLU:
					return (Color){0, 255, 255, 255};
				default:
					return (Color){255, 255, 255, 255};
			}
		}

		return (Color){255, 255, 255, 255};
	}

	inline void Run(CTFPlayer* pLocal)
	{
		if (!settings.esp.enabled)
			return;

		if (!pLocal)
			return;

		helper::draw::SetColor(255, 255, 255, 255);

		// start at 1 because 0 is the world
		for (int i = 1; i < helper::engine::GetMaxClients(); i++)
		{
			IClientEntity* clientEnt = interfaces::EntityList->GetClientEntity(i);
			if (!clientEnt)
				continue;

			auto entity = static_cast<CTFPlayer*>(clientEnt);
			if (!entity || !entity->IsPlayer())
				continue;

			if (entity->GetIndex() == pLocal->GetIndex())
				continue;

			if (entity->IsDormant() || !entity->IsAlive())
				continue;

			if (entity->InCond(ETFCond::TF_COND_CLOAKED) && settings.esp.ignorecloaked)
				continue;

			//interfaces::vstdlib->ConsolePrintf("Health: %d\n", entity->GetHealth());

			Vector origin = entity->GetAbsOrigin();
			Vector feet; // fuck i need to sleep
			if (!helper::engine::WorldToScreen(origin, feet))
				continue;

			Vector head;
			if (!helper::engine::WorldToScreen(origin + Vector{0, 0, entity->m_vecMaxs().z}, head))
				continue;

			int h = (feet - head).Length();
			int w = h * 0.3;

			helper::draw::SetColor(GetColor(entity));
			helper::draw::OutlinedRect(head.x - w, feet.y - h, feet.x + w, feet.y);
		}
	}
};