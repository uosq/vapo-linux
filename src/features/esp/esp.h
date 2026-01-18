#pragma once

#include "../../sdk/interfaces/interfaces.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/classes/cbaseobject.h"
#include "../../sdk/definitions/eteam.h"
#include "../entitylist/entitylist.h"
#include "../../settings.h"

namespace ESP
{
	inline Color GetPlayerColor(CBaseEntity* player)
	{
		if (player == EntityList::m_pAimbotTarget)
			return settings.colors.aimbot_target;

		int team = player->m_iTeamNum();

		if (player->IsPlayer())
		{
			switch (team)
			{
				case ETeam::TEAM_RED:
					return settings.colors.red_team;
				case ETeam::TEAM_BLU:
					return settings.colors.blu_team;
				default: break;
			}
		}

		return (Color){255, 255, 255, 255};
	}

	// Had to make a separate one
	// Because m_iTeamNum for some reason is inverted on buildings
	inline Color GetBuildingColor(CBaseObject* building)
	{
		if (reinterpret_cast<CBaseEntity*>(building) == EntityList::m_pAimbotTarget)
			return settings.colors.aimbot_target;

		CTFPlayer* builder = HandleAs<CTFPlayer*>(building->m_hBuilder());
		if (builder != nullptr)
		{
			int team = builder->m_iTeamNum();
			switch (team)
			{
				case ETeam::TEAM_RED:
					return settings.colors.red_team;
				case ETeam::TEAM_BLU:
					return settings.colors.blu_team;
				default: break;
			}
		}
		
		return (Color){255, 255, 255, 255};
	}

	inline bool IsValidPlayer(CTFPlayer* pLocal, CBaseEntity* entity)
	{
		if (entity == nullptr)
			return false;

		if (entity->IsDormant())
			return false;

		if (!entity->IsPlayer())
			return false;

		if (!interfaces::CInput->CAM_IsThirdPerson() && entity->GetIndex() == pLocal->GetIndex())
			return false;

		CTFPlayer* player = static_cast<CTFPlayer*>(entity);
		if (!player)
			return false;

		if (!player->IsAlive())
			return false;

		if (player->InCond(TF_COND_CLOAKED) && settings.esp.ignorecloaked)
			return false;

		return true;
	}

	inline bool IsValidBuilding(CTFPlayer* pLocal, CBaseObject* entity)
	{
		if (entity == nullptr)
			return false;

		if (entity->IsDormant())
			return false;

		return true;
	}

	inline void PaintBox(Color color, Vector top, Vector bottom, int w, int h)
	{
		helper::draw::SetColor(color);
		helper::draw::OutlinedRect(top.x - w, bottom.y - h, bottom.x + w, bottom.y);
	}

	inline void PaintName(Color color, Vector top, int w, int h, std::string name)
	{
		int textw, texth;
		helper::draw::GetTextSize(name, textw, texth);
		helper::draw::TextShadow(top.x - (textw*0.5f), top.y - texth - 2, color, name);
	}

	inline void Run(CTFPlayer* pLocal)
	{
		if (!helper::engine::IsInMatch() || !settings.esp.enabled)
			return;

		Color white {255, 255, 255, 255};
		helper::draw::SetFont(fontManager.GetCurrentFont());

		for (auto entity : EntityList::m_vecPlayers)
		{
			if (!IsValidPlayer(pLocal, entity))
				continue;

			Vector origin = entity->GetAbsOrigin();
			Vector feet; // fuck i need to sleep
			if (!helper::engine::WorldToScreen(origin, feet))
				continue;

			Vector head;
			if (!helper::engine::WorldToScreen(origin + Vector{0, 0, entity->m_vecMaxs().z}, head))
				continue;

			// name, class, etc
			CTFPlayer* player = static_cast<CTFPlayer*>(entity);
			if (player == nullptr)
				continue;

			int h = (feet - head).Length();
			int w = h * 0.3;

			Color color = GetPlayerColor(entity);

			if (settings.esp.box)
				PaintBox(color, head, feet, w, h);
	
			// name
			if (settings.esp.name)
				PaintName(white, head, w, h, player->GetName());
		}

		if (settings.esp.buildings)
		{
			// skip players
			for (auto entity : EntityList::m_vecBuildings)
			{
				if (!IsValidBuilding(pLocal, entity))
					return;

				Vector bottom;
				Vector origin = entity->GetAbsOrigin();
				if (!helper::engine::WorldToScreen(origin, bottom))
					continue;

				Vector top;
				Vector max = origin + Vector(0, 0, entity->m_vecMaxs().z);
				if (!helper::engine::WorldToScreen(max, top))
					continue;

				int h = (bottom - top).Length();
				int w = static_cast<CBaseEntity*>(entity)->IsTeleporter() ? h * 2.0f : h * 0.3f;

				Color color = GetBuildingColor(entity);

				if (settings.esp.box)
					PaintBox(color, top, bottom, w, h);

				if (settings.esp.name)
					PaintName(white, top, w, h, std::string(entity->GetName()));
			}
		}
	}
};