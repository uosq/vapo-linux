#pragma once

#include "../../../sdk/classes/player.h"
#include "../../../sdk/classes/cbaseobject.h"
#include "../../../settings.h"

struct PotentialTarget
{
	Vector dir;
	Vector center;
	float distance;
	float dot;
	CBaseEntity* entity;
};

namespace AimbotUtils
{
	inline bool IsValidEntity(CTFPlayer* pLocal, CTFPlayer* entity, bool checkTeam = true)
	{
		if (entity == nullptr)
			return false;

		if (entity == pLocal)
			return false;

		if (checkTeam && entity->m_iTeamNum() == pLocal->m_iTeamNum())
			return false;

		if (entity->IsDormant())
			return false;

		if (settings.esp.ignorecloaked && entity->InCond(ETFCond::TF_COND_CLOAKED))
			return false;

		if (entity->IsPlayer())
		{
			if (!entity->IsAlive())
				return false;

			if (entity->IsUbercharged())
				return false;
	
			if (entity->IsGhost())
				return false;
		}

		if (entity->IsDispenser() || entity->IsSentry() || entity->IsTeleporter())
		{
			auto* building = reinterpret_cast<CBaseObject*>(entity);
			if (building == nullptr)
				return false;

			if (building->m_iHealth() <= 0)
				return false;
		}

		return true;
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
};