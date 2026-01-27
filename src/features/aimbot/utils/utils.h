#pragma once

#include "../../../sdk/classes/player.h"
#include "../../../sdk/classes/cbaseobject.h"
#include "../../../settings.h"
#include "../../../sdk/definitions/cgametrace.h"
#include "../../../sdk/definitions/ctracefilters.h"
#include "../../../sdk/helpers/helper.h"
#include "../../entitylist/entitylist.h"
#include "../../visuals/customfov/customfov.h"

#define ARRAYSIZE(x) sizeof((x))/sizeof((x[0]))

struct PotentialTarget
{
	Vector dir;
	Vector center;
	float distance;
	float fov;
	CBaseEntity* entity;
};

struct AimbotState
{
	bool running = false;
	bool shouldSilent = false;
	//std::vector<Vector> targetPath;
	Vector angle = {0, 0, 0};
};

static void ClearAimbotState(AimbotState& state)
{
	//state.angle.Set(0, 0, 0);
	state.shouldSilent = false;
	state.running = false;
	//state.targetPath.clear();
}

namespace AimbotUtils
{
	inline bool IsValidEntity(CBaseEntity* entity, int localTeam, bool ignoreTeammate = true)
	{
		if (entity == nullptr)
			return false;

		if (entity->IsDormant())
			return false;

		if (ignoreTeammate && entity->m_iTeamNum() == localTeam)
			return false;

		if (entity->IsPlayer())
		{
			CTFPlayer* player = static_cast<CTFPlayer*>(entity);
			if (player == nullptr)
				return false;

			if (!player->IsAlive())
				return false;

			if (player->IsUbercharged())
				return false;
	
			if (player->IsGhost())
				return false;

			if (settings.aimbot.ignorecloaked && player->InCond(ETFCond::TF_COND_CLOAKED))
				return false;

			if (settings.aimbot.ignorebonked && player->InCond(ETFCond::TF_COND_BONKED))
				return false;

			if (settings.aimbot.ignoreubered && player->InCond(ETFCond::TF_COND_INVULNERABLE))
				return false;

			if (settings.aimbot.ignorehoovy && player->m_iClass() == ETFClass::TF_CLASS_HEAVYWEAPONS && (player->GetFlags() & FL_DUCKING))
				return false;

			return true;
		}

		if (entity->IsBuilding())
		{
			auto* building = static_cast<CBaseObject*>(entity);
			if (building == nullptr)
				return false;

			if (building->m_iHealth() <= 0)
				return false;

			return true;
		}

		return false;
	}

	// Is this optimized? absolutely fucking not
	// I need to think of a better way
	// I should probably check bones
	inline bool GetVisiblePoint(Vector& out, CTFPlayer* pLocal, Vector origin, Vector mins, Vector maxs)
	{
		static float points[] = {0.15f, 0.35f, 0.5f, 0.75f, 0.85f};
		static constexpr int points_size = ARRAYSIZE(points);
		CGameTrace trace;
		CTraceFilterWorldAndPropsOnly filter;
		filter.pSkip = pLocal;

		Vector absMax, absMin;
		absMax = origin + maxs;
		absMin = origin + mins;

		Vector eyePos = pLocal->GetAbsOrigin() + pLocal->m_vecViewOffset();

		for (int x = 0; x < points_size; x++)
			for (int y = 0; y < points_size; y++)
				for (int z = 0; z < points_size; z++)
				{
					Vector point;
					point.x = absMin.x + (absMax.x - absMin.x) * points[x];
					point.y = absMin.y + (absMax.y - absMin.y) * points[y];
					point.z = absMin.z + (absMax.z - absMin.z) * points[z];

					helper::engine::Trace(eyePos, point, MASK_SHOT_HULL, &filter, &trace);

					if (trace.fraction == 1.0f)
					{
						out = point;
						return true;
					}
				}

		return false;
	}

	inline std::vector<CBaseEntity*> GetTargets(int localTeam)
	{
		const auto& enemies = EntityList::GetEnemies();
		std::vector<CBaseEntity*> targets;

		for (const auto& enemy : enemies)
		{
			if (enemy == nullptr)
				continue;

			// pass no team and dont check team
			// must be pretty obvious why
			if (!IsValidEntity(enemy, 0, false))
				continue;

			targets.emplace_back(enemy);
		}

		return targets;
	}

	inline std::string GetAimbotModeName()
	{
		switch(settings.aimbot.mode)
		{
			case AimbotMode::PLAIN: return "Plain";
			case AimbotMode::SMOOTH: return "Smooth";
			case AimbotMode::ASSISTANCE: return "Assistance";
			case AimbotMode::SILENT: return "Silent";
			case AimbotMode::PSILENT: return "pSilent";
			default: return "Invalid";
                }
        }

	inline std::string GetTeamModeName()
	{
		switch(settings.aimbot.teamMode)
		{
			case TeamMode::ONLYENEMY: return "Only Enemy";
			case TeamMode::ONLYTEAMMATE: return "Only Teammate";
			case TeamMode::BOTH: return "Both";
			default: return "Invalid";
                }
        }

	inline bool CanDamageWithSniperRifle(CTFPlayer* pLocal, CBaseEntity* pTarget, CTFWeaponBase* pWeapon)
	{
		if (pLocal == nullptr || pTarget == nullptr || pWeapon == nullptr)
			return false;

		if (!pWeapon->IsSniperRifle())
			return true;

		if (pTarget->IsPlayer())
		{
			CTFPlayer* player = static_cast<CTFPlayer*>(pTarget);
			if (player == nullptr)
				return false;

			int health = player->GetHealth();
			return static_cast<CTFSniperRifle*>(pWeapon)->GetChargedDamage() >= health;
		}

		if (pTarget->IsBuilding())
		{
			CBaseObject* obj = static_cast<CBaseObject*>(pTarget);
			if (obj == nullptr)
				return false;

			int health = obj->m_iHealth();
			return static_cast<CTFSniperRifle*>(pWeapon)->GetChargedDamage() >= health;
		}

		return false;
	}

	inline float GetAimbotFovScaled(CTFPlayer* pLocal)
	{
		float cameraFOV = customfov.GetFov();

		float radAimbotHalf = DEG2RAD(settings.aimbot.fov / 2.0f);
		float radPlayerHalf = DEG2RAD(cameraFOV / 2.0f);
		float radBaseHalf   = DEG2RAD(90.0f / 2.0f);

		float scaledRad = atan(tan(radAimbotHalf) * (tan(radPlayerHalf) / tan(radBaseHalf)));

		return RAD2DEG(scaledRad) * 2.0f;
	}
};