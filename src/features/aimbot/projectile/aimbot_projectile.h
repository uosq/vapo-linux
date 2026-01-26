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
#include "../../entitylist/entitylist.h"
#include <cmath>
#include <vector>

struct ProjectileInfo_t
{
	float speed = 0;
	float gravity = 0;
	float primetime = 0;
	float damage_radius = 0;
	float lifetime = 60.0f;
	bool simple_trace = false;
	Vector offset{};
	Vector hull{6, 6, 6};
};

struct AimbotProjectile
{
	bool GetProjectileInfo(ProjectileInfo_t& info, CTFPlayer* owner, CTFWeaponBase* pWeapon)
	{
		if (owner == nullptr || pWeapon == nullptr)
			return false;

		bool bDucking = owner->GetFlags() & FL_DUCKING;
		float gravity = interfaces::Cvar->FindVar("sv_gravity")->GetFloat()/800;

		int id = pWeapon->GetWeaponID();

		int iTickBase = owner->GetTickBase();
		float flTickBase = TICKS_TO_TIME(iTickBase);

		switch(id)
		{
			case TF_WEAPON_ROCKETLAUNCHER:
			case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
			{
				info.hull.Set();
				info.speed = owner->InCond(TF_COND_RUNE_PRECISION) ? 3000 : AttributeHookValue(1100, "mult_projectile_speed", pWeapon, nullptr, true);
				info.offset.x = 23.5f;
				info.offset.y = AttributeHookValue(0, "centerfire_projectile", pWeapon, nullptr, true) == 1 ? 0 : 12;
				info.offset.z = bDucking ? 8 : -3;
				info.damage_radius = id == TF_WEAPON_ROCKETLAUNCHER ? 146 : 44;
				info.simple_trace = true;
				return true;
			}

			case TF_WEAPON_PARTICLE_CANNON:
			case TF_WEAPON_RAYGUN:
			case TF_WEAPON_DRG_POMSON:
			{
				bool bIsCowMangler = id == TF_WEAPON_PARTICLE_CANNON;
				info.offset.Set(23.5, 8, bDucking ? 8 : -3);
				info.speed = bIsCowMangler ? 1100 : 1200;
				info.hull = bIsCowMangler ? Vector(0, 0, 0) : Vector(1, 1, 1);
				info.simple_trace = true;
				return true;
			}

			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_CANNON:
			{
				bool bIsCannon = id == TF_WEAPON_CANNON;
				float mortar = bIsCannon ? AttributeHookValue(0.f, "grenade_launcher_mortar_mode", pWeapon, nullptr, true) : 0;
				info.speed = AttributeHookValue(owner->InCond(TF_COND_RUNE_PRECISION) ? 3000 : AttributeHookValue(1200, "mult_projectile_range", pWeapon, nullptr, true), "mult_projectile_range", pWeapon, nullptr, true);
				info.gravity = gravity;
				return true;
			}

			case TF_WEAPON_PIPEBOMBLAUNCHER:
			{
				info.offset.Set(16, 8, -6);
				info.gravity = gravity;
				
				float charge = 0.0f;
				float m_flChargeBeginTime = ((CTFPipebombLauncher*)pWeapon)->m_flChargeBeginTime();
				if (m_flChargeBeginTime > flTickBase)
					charge = 0.0f;
				else
					charge = flTickBase - m_flChargeBeginTime;

				info.speed = AttributeHookValue(Math::RemapVal(0, 0, AttributeHookValue(4.0f, "stickybomb_charge_rate", pWeapon, nullptr, true), 900, 2400, true), "mult_projectile_range", pWeapon, nullptr, true);
				return true;
			}

			case TF_WEAPON_FLAREGUN:
			{
				info.offset.Set(23.5, 12, bDucking ? 8 : -3);
				info.hull.Set(0, 0, 0);
				info.speed = AttributeHookValue(2000, "mult_projectile_speed", pWeapon, nullptr, true);
				info.gravity = 0.01f;
				info.lifetime = 0.3 * gravity;
				return true;
			}

			case TF_WEAPON_FLAREGUN_REVENGE:
			{
				info.offset.Set(23.5, 12, bDucking ? 8 : -3);
				info.hull.Set(0, 0, 0);
				info.speed = 3000;
				return true;
			}

			case TF_WEAPON_COMPOUND_BOW:
			{
				info.offset.Set(23.5, 12, -3);
				info.hull.Set(1, 1, 1);

				float flchargebegintime = static_cast<CTFPipebombLauncher*>(pWeapon)->m_flChargeBeginTime();
				float charge = 0.0f;
				if (flchargebegintime > 0)
					charge = TICKS_TO_TIME( owner->GetTickBase()) - flchargebegintime;

				info.speed = Math::RemapVal(charge, 0, 1, 1800, 2600);
				info.gravity = Math::RemapVal(charge, 0, 1, 0.5, 0.1) * gravity;
				info.lifetime = 10;
				return true;
			}

			case TF_WEAPON_CROSSBOW:
			case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
			{
				bool isCrossbow = id == TF_WEAPON_CROSSBOW;
				info.offset.Set(23.5, 12, -3);
				info.hull = isCrossbow ? Vector(3, 3, 3) : Vector(1, 1, 1);
				info.speed = 2400;
				info.gravity = gravity * 0.2;
				info.lifetime = 10;
				return true;
			}

			case TF_WEAPON_SYRINGEGUN_MEDIC:
			{
				info.offset.Set(16, 6, -8);
				info.hull.Set(1, 1, 1);
				info.speed = 1000;
				info.gravity = 0.3 * gravity;
				return true;
			}

			case TF_WEAPON_FLAMETHROWER:
			{
				static ConVar* tf_flamethrower_size = interfaces::Cvar->FindVar("tf_flamethrower_boxsize");
				if (!tf_flamethrower_size)
					return false;
				
				float flhull = tf_flamethrower_size->GetFloat();
				info.offset.Set(40, 5, 0);
				info.hull.Set(flhull, flhull, flhull);
				info.speed = 1000;
				info.lifetime = 0.285;
				info.simple_trace = true;
				return true;
			}

			case TF_WEAPON_FLAME_BALL:
			{
				info.offset.Set(3, 7, -9);
				info.hull.Set(1, 1, 1);
				info.speed = 3000;
				info.lifetime = 0.18;
				info.gravity = 0;
				info.simple_trace = true;
				return true;
			}

			case TF_WEAPON_CLEAVER:
			{
				info.offset.Set(16, 8, -6);
				info.hull.Set(1, 1, 10); // wtf is this 10?
				info.gravity = 1;
				info.lifetime = 2.2;
				return true;
			}

			case TF_WEAPON_BAT_WOOD:
			case TF_WEAPON_BAT_GIFTWRAP:
			{
				ConVar* tf_scout_stunball_base_speed = interfaces::Cvar->FindVar("tf_scout_stunball_base_speed");
				if (!tf_scout_stunball_base_speed)
					return false;

				info.speed = tf_scout_stunball_base_speed->GetFloat();
				info.gravity = 1;
				info.lifetime = gravity;
				return true;
			}

			case TF_WEAPON_JAR:
			case TF_WEAPON_JAR_MILK:
			{
				info.offset.Set(16, 8, -6);
				info.speed = 1000;
				info.gravity = 1;
				info.lifetime = 2.2;
				info.hull.Set(3, 3, 3);
				return true;
			}

			case TF_WEAPON_JAR_GAS:
			{
				info.offset.Set(16, 8, -6);
				info.speed = 2000;
				info.gravity = 1;
				info.lifetime = 2.2;
				info.hull.Set(3, 3, 3);
				return true;
			}

			case TF_WEAPON_LUNCHBOX:
			{
				info.offset.z = -8;
				info.hull.Set(17, 17, 7);
				info.speed = 500;
				info.gravity = 1 * gravity;
				return true;
			}
			default: return false;
		}

		return false;
	}

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

	bool CheckTrajectory(CBaseEntity* target, Vector startPos, Vector targetPos, Vector angle, ProjectileInfo_t info, float gravity)
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

	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, AimbotState& state)
	{
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

		for (auto entity : EntityList::GetEnemies())
		{
			if (!AimbotUtils::IsValidEntity(entity, localTeam))
				continue;

			Vector center;// = entity->GetCenter();
			{
				if (entity->IsPlayer())
					center = static_cast<CTFPlayer*>(entity)->GetCenter();
				else if (entity->IsBuilding())
					center = reinterpret_cast<CBaseObject*>(entity)->GetCenter();
				else
					center = entity->GetAbsOrigin();
			}

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

			if (time > settings.aimbot.max_sim_time)
				continue;

			Vector lastPos;
			std::vector<Vector> path;
			Vector angle;

			if (target.entity->IsPlayer())
			{
				CTFPlayer* player = static_cast<CTFPlayer*>(target.entity);
				if (player == nullptr)
					continue;

				PlayerPrediction::Predict(player, time, path);
	
				// something went wrong
				if (path.empty())
					continue;
	
				lastPos = path.back();
	
				float aimOffset = GetInitialOffset(player, pWeapon);
				if (aimOffset > 0)
					lastPos.z += aimOffset;
			}
			else if (target.entity->IsBuilding())
			{
				lastPos = reinterpret_cast<CBaseObject*>(target.entity)->GetCenter();
				path.emplace_back(target.entity->GetAbsOrigin());
			}
			else
			{
				continue;
			}

			if (info.simple_trace)
			{
				Vector dir = (lastPos - shootPos);
				dir.Normalize();
				angle = dir.ToAngle();

				if (!CheckTrajectory(target.entity, shootPos, lastPos, angle, info, 0))
					continue;
			} else
			{
				if (!SolveBallisticArc(angle, shootPos, lastPos, info.speed, gravity))
					continue;

				if (!CheckTrajectory(target.entity, shootPos, lastPos, angle, info, gravity))
					continue;
			}

			if (settings.aimbot.autoshoot)
				pCmd->buttons |= IN_ATTACK;

			if (helper::localplayer::CanShoot(pLocal, pWeapon, pCmd))
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
							//interfaces::Cvar->ConsolePrintf("Charge: %f\n", flchargebegintime);

							float charge = flchargebegintime > 0.f ? TICKS_TO_TIME(pLocal->GetTickBase()) - flchargebegintime : 0.f;
							if (charge > 0.0f)
								pCmd->buttons &= ~IN_ATTACK;
							break;
						}

						default: break;
					}
				}
			}

			if (helper::localplayer::IsAttacking(pLocal, pWeapon, pCmd))
			{
				pCmd->viewangles = angle;
				state.angle = angle;

				AimbotMode mode = settings.aimbot.mode;

				if (mode == AimbotMode::PSILENT)
					state.shouldSilent = true;

				if (mode == AimbotMode::PLAIN)
					interfaces::Engine->SetViewAngles(angle);
			}
			
			EntityList::m_pAimbotTarget = target.entity;
			state.running = true;
			return;
		}
	}
};