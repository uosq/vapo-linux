#pragma once

#include "../../sdk/interfaces/interfaces.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/classes/cbaseobject.h"
#include "../../sdk/definitions/eteam.h"
#include "../../settings.h"

/*
enum TextAlignment {
	TEXTALIGN_NONE = 0,
	TEXTALIGN_LEFT,
	TEXTALIGN_RIGHT,
	TEXTALIGN_BOTTOM,
	TEXTALIGN_TOP,
	TEXTALIGN_COUNT,
};
*/

namespace ESP
{
	inline Color GetPlayerColor(CBaseEntity* player)
	{
		int team = player->m_iTeamNum();

		if (player->IsPlayer())
		{
			switch (team)
			{
				case ETeam::TEAM_RED:
					return (Color){255, 0, 0, 255};
				case ETeam::TEAM_BLU:
					return (Color){0, 255, 255, 255};
				default:
					break;
			}
		}

		return (Color){255, 255, 255, 255};
	}

	inline Color GetBuildingColor(CBaseObject* building)
	{
		CTFPlayer* builder = HandleAs<CTFPlayer>(building->m_hBuilder());
		if (builder != nullptr)
		{
			int team = builder->m_iTeamNum();
			switch (team)
			{
				case ETeam::TEAM_RED:
					return (Color){255, 0, 0, 255};
				case ETeam::TEAM_BLU:
					return (Color){0, 255, 255, 255};
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

		//if (entity->m_iHealth() <= 0)
			//return false;

		return true;
	}

	inline void PaintBox(Color color, Vector top, Vector bottom, int w, int h)
	{
		helper::draw::SetColor(color);
		helper::draw::OutlinedRect(top.x - w, bottom.y - h, bottom.x + w, bottom.y);
	}

	inline void PaintName(Color color, Vector top, int w, int h, std::string name)
	{
		helper::draw::TextShadow(top.x + w + 2, top.y, color, name);
	}

	inline void Run(CTFPlayer* pLocal)
	{
		if (!settings.esp.enabled)
			return;

		if (!pLocal)
			return;

		Color white {255, 255, 255, 255};
		helper::draw::SetColor(white);

		int maxclients = helper::engine::GetMaxClients();

		// start at 1 because 0 is the world
		for (int i = 1; i <= maxclients; i++)
		{
			IClientEntity* clientEnt = interfaces::EntityList->GetClientEntity(i);
			if (!clientEnt)
				continue;

			auto entity = static_cast<CBaseEntity*>(clientEnt);
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
			for (int i = maxclients; i <= interfaces::EntityList->GetHighestEntityIndex(); i++)
			{
				IClientEntity* clientEnt = interfaces::EntityList->GetClientEntity(i);
				if (clientEnt == nullptr)
					continue;

				CBaseEntity* baseEnt = static_cast<CBaseEntity*>(clientEnt);
				if (baseEnt == nullptr)
					continue;

				if (!(baseEnt->IsSentry() || baseEnt->IsTeleporter() || baseEnt->IsDispenser()))
					continue;

				// fucking stupid c++
				CBaseObject* entity = reinterpret_cast<CBaseObject*>(baseEnt);
				if (entity == nullptr)
					continue;

				if (!IsValidBuilding(pLocal, entity))
					return;

				Vector bottom;
				Vector origin = entity->GetAbsOrigin();
				if (!helper::engine::WorldToScreen(origin, bottom))
					continue;

				Vector top;
				Vector max = origin + Vector(0, 0, entity->m_vecBuildMaxs().z);
				if (!helper::engine::WorldToScreen(max, top))
					continue;

				int h = (bottom - top).Length();
				int w = baseEnt->IsTeleporter() ? h * 2.0f : h * 0.3f;

				Color color = GetBuildingColor(entity);

				if (settings.esp.box)
					PaintBox(color, top, bottom, w, h);

				if (settings.esp.name)
					PaintName(white, top, w, h, std::string(entity->GetName()));
			}
		}
	}
};