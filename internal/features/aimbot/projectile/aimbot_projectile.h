#pragma once

#include "../utils/utils.h"
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
#include "../../prediction/prediction.h"
#include <cmath>
#include <vector>

struct AimbotProjectile
{
	// Offset before any multipointing is applied
	float GetInitialOffset(CTFPlayer* target, CTFWeaponBase* pWeapon)
	{
		switch (pWeapon->GetWeaponID())
		{
			case TF_WEAPON_ROCKETLAUNCHER:
			case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
				return 10.0f;

			case TF_WEAPON_COMPOUND_BOW:
				return target->m_vecMaxs().z;
		}

		return target->m_vecMaxs().z * 0.5f;
	}

	bool SolveBallisticArc(Vector &outAngle, Vector p0, Vector p1, float speed, float gravity)
	{
		Vector diff = p1 - p0;
		float dx = diff.Length2D();
		float dy = diff.z;
		float speed2 = speed * speed;
		float g = gravity;

		float root = speed2 * speed2 - g * (g * dx * dx + 2 * dy * speed2);
		if (root < 0)
			return false;

		float angle, yaw, pitch;
		angle = atan((speed2 - sqrt(root)) / (g * dx));
		yaw = RAD2DEG(atan2(diff.y, diff.x));
		pitch = RAD2DEG(-angle);

		outAngle.Set(pitch, yaw);
		return true;
	}

	bool CheckTrajectory(CTFPlayer* target, Vector startPos, Vector targetPos, Vector angle, ProjectileInfo_t info, float gravity)
	{
		if (!target)
			return false;

		// step
		float accuracy = 5.0f;

		float distance = (targetPos - startPos).Length();
		float totalTime = distance / info.speed;

		Vector velocity;
		Math::AngleVectors(angle, &velocity);
		
		velocity *= info.speed;

		CGameTrace trace;
		CTraceFilterWorldAndPropsOnly filter;
		filter.pSkip = target;

		Vector min{-info.hull.x, -info.hull.y, -info.hull.z};

		for (int i = 1; i <= accuracy; i++)
		{
			float t = (static_cast<float>(i) / accuracy) * totalTime;

			Vector pos = startPos + (velocity * t);
			pos.z -= (0.5 * gravity * t * t);

			float prevT = (static_cast<float>(i - 1) / accuracy) * totalTime;
			Vector prevPos = startPos + (velocity * prevT);
			prevPos.z -= (0.5f * gravity * prevT * prevT);

			helper::engine::TraceHull(prevPos, pos, min, info.hull, MASK_SHOT_HULL, &filter, &trace);

			if (trace.fraction < 1.0f)
				return false;
		}

		return true;
	}

	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, std::vector<Vector> &targetPath, Vector &outAngle, bool &running, bool &shouldSilent)
	{
		if (pCmd->weaponselect)
			return;

		ProjectileInfo_t info;
		if (!GetProjectileInfo(info, pLocal, pWeapon))
			return;

		int localTeam = pLocal->m_iTeamNum();
		Vector shootPos = pLocal->GetEyePos();

		Vector viewAngles;
		interfaces::Engine->GetViewAngles(viewAngles);

		Vector viewForward;
		Math::AngleVectors(viewAngles, &viewForward);
		viewForward.Normalize();

		std::vector<PotentialTarget> targets;

		float fovRad = DEG2RAD(settings.aimbot.fov);
		float minDot = cosf(fovRad);

		CGameTrace trace;
		CTraceFilterHitscan filter;
		filter.pSkip = pLocal;

		static ConVar* sv_gravity = interfaces::Cvar->FindVar("sv_gravity");
		if (!sv_gravity)
			return;

		float gravity = sv_gravity->GetFloat() * 0.5f * info.gravity;

		for (int i = 1; i < helper::engine::GetMaxClients(); i++)
		{
			CTFPlayer* entity = (CTFPlayer*)interfaces::EntityList->GetClientEntity(i);

			if (!AimbotUtils::IsValidEntity(pLocal, entity))
				continue;

			Vector center = entity->GetCenter();
			Vector dir = center - shootPos;
			float distance = dir.Normalize();

			if (distance >= 2048.f)
				continue;

			float dot = dir.Dot(viewForward);

			if (dot < minDot)
				continue;

			targets.emplace_back(PotentialTarget{dir, center, distance, dot, entity});
		}

		if (targets.empty())
			return;

		AimbotUtils::QuickSort(targets, 0, targets.size() - 1);

		for (const auto& target : targets)
		{
			float time = (target.distance/info.speed);
			//interfaces::vstdlib->ConsolePrintf("Time: %f\n", time);

			if (time > settings.aimbot.max_sim_time)
				continue;

			std::vector<Vector> path;
			PlayerPrediction::Predict((CTFPlayer*)target.entity, time, path);

			// something went wrong
			if (path.empty())
				continue;

			Vector lastPos = path.back();
			if (!std::isfinite(lastPos.x) || !std::isfinite(lastPos.y) || !std::isfinite(lastPos.z))
                		continue;

			float aimOffset = GetInitialOffset((CTFPlayer*)target.entity, pWeapon);
			if (aimOffset > 0)
				lastPos.z += aimOffset;

			Vector angle;

			if (info.simple_trace)
			{
				Vector dir = (lastPos - shootPos);
				dir.Normalize();
				angle = dir.ToAngle();

				if (!CheckTrajectory((CTFPlayer*)target.entity, shootPos, lastPos, angle, info, 0))
					continue;
			} else
			{
				if (!SolveBallisticArc(angle, shootPos, lastPos, info.speed, gravity))
					continue;

				if (!CheckTrajectory((CTFPlayer*)target.entity, shootPos, lastPos, angle, info, gravity))
					continue;
			}

			if (settings.aimbot.autoshoot)
				pCmd->buttons |= IN_ATTACK;

			if (helper::localplayer::CanShoot(pLocal, pWeapon))
			{
				if ((pCmd->buttons & IN_ATTACK))
				{
					int weaponID = pWeapon->GetWeaponID();
					switch (weaponID)
					{
						case TF_WEAPON_COMPOUND_BOW:
						case TF_WEAPON_PIPEBOMBLAUNCHER:
						{
							float flchargebegintime = static_cast<CTFPipebombLauncher*>(pWeapon)->m_flChargeBeginTime();
							interfaces::Cvar->ConsolePrintf("Charge: %f\n", flchargebegintime);

							float charge = flchargebegintime > 0.f ? TICKS_TO_TIME(pLocal->GetTickBase()) - flchargebegintime : 0.f;
							if (charge > 0.0f)
								pCmd->buttons &= ~IN_ATTACK;
							break;
						}

						default: break;
					}
				}

				targetPath = path;
				pCmd->viewangles = angle;
				outAngle = angle;
				shouldSilent = true;
			}
			
			running = true;
			return;
		}
	}
};