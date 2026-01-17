#pragma once

#include "../../../sdk/classes/player.h"
#include "../../../sdk/classes/cbaseobject.h"
#include "../../../settings.h"
#include "../../../sdk/definitions/cgametrace.h"
#include "../../../sdk/definitions/ctracefilters.h"
#include "../../../sdk/helpers/helper.h"
#include "../../entitylist/entitylist.h"

#define ARRAYSIZE(x) sizeof((x))/sizeof((x[0]))

struct PotentialTarget
{
	Vector dir;
	Vector center;
	float distance;
	float dot;
	CBaseEntity* entity;
};

struct AimbotState
{
	bool running = false;
	bool shouldSilent = false;
	std::vector<Vector> targetPath;
	Vector angle = {0, 0, 0};
};

static void ClearAimbotState(AimbotState& state)
{
	//state.angle.Set(0, 0, 0);
	state.shouldSilent = false;
	state.running = false;
	state.targetPath.clear();
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

			if (settings.esp.ignorecloaked && player->InCond(ETFCond::TF_COND_CLOAKED))
				return false;

			return true;
		}

		if (entity->IsBuilding())
		{
			auto* building = reinterpret_cast<CBaseObject*>(entity);
			if (building == nullptr)
				return false;

			if (building->m_iHealth() <= 0)
				return false;

			return true;
		}

		return false;
	}

	// I forgor :skull:
	// Source: https://www.geeksforgeeks.org/cpp/cpp-program-to-sort-the-elements-of-an-array-in-ascending-order/
	inline void QuickSort(std::vector<PotentialTarget>& targets, int low, int high)
	{
		if (low >= high)
			return;

		int start = low;
		int end   = high;

		float pivot = targets[low + (high - low) / 2].dot;

		while (start <= end)
		{
			while (targets[start].dot > pivot)
				start++;

			while (targets[end].dot < pivot)
				end--;

			if (start <= end)
			{
				std::swap(targets[start], targets[end]);
				start++;
				end--;
			}
		}

		if (low < end)
			QuickSort(targets, low, end);

		if (start < high)
			QuickSort(targets, start, high);
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
};