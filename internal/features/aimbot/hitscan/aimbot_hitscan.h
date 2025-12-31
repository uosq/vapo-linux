#pragma once

#include "../../../sdk/definitions/cusercmd.h"
#include "../../../sdk/classes/player.h"
#include "../../../sdk/classes/weaponbase.h"
#include "../../../sdk/helpers/helper.h"
#include "../../../sdk/handle_utils.h"
#include "../../../settings.h"

#include <cmath>
#include <vector>

struct PotentialTarget
{
	float dot;
	CBaseEntity* entity;
};

struct AimbotHitscan
{
	bool IsValidEntity(CTFPlayer* pLocal, CTFPlayer* entity)
	{
		if (!entity)
			return false;

		if (entity == pLocal)
			return false;

		if (entity->IsDormant())
			return false;

		if (!entity->IsAlive())
			return false;

		if (settings.esp.ignorecloaked && entity->InCond(ETFCond::TF_COND_CLOAKED))
			return false;

		return true;
	}

	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
	{
		int localTeam = pLocal->m_iTeamNum();
		Vector shootPos = pLocal->GetEyePos();

		Vector viewAngles;
		interfaces::engine->GetViewAngles(viewAngles);

		Vector viewForward;
		Math::AngleVectors(viewAngles, &viewForward);
		viewForward.Normalize();

		std::vector<PotentialTarget> targets;

		float fovRad = DEG2RAD(settings.aimbot.fov);
		float minDot = cosf(fovRad);

		for (int i = 1; i < helper::engine::GetMaxClients(); i++)
		{
			CTFPlayer* entity = (CTFPlayer*)interfaces::entitylist->GetClientEntity(i);

			if (!IsValidEntity(pLocal, entity))
				continue;

			Vector dir = entity->GetCenter() - shootPos;
			float distance = dir.Normalize();

			if (distance >= 2048.f)
				continue;

			float dot = dir.Dot(viewForward);

			// outside fov
			if (dot < minDot)
				continue;

			targets.emplace_back(PotentialTarget{dot, entity});
		}

		if (targets.empty())
			return;
	}
};