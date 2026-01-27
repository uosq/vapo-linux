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

#include <cmath>
#include <vector>

enum class HitscanOffset
{
	HEAD = 0,
	CHEST,
	//PELVIS
	// legs are unreliable
};

struct AimbotHitscan
{
	HitscanOffset GetInitialOffset(CTFPlayer* pLocal, CTFWeaponBase* pWeapon)
	{
		if (pWeapon == nullptr)
			return HitscanOffset::CHEST;

		switch(pWeapon->GetWeaponID())
		{
			case TF_WEAPON_REVOLVER:
			{
				if (pWeapon->CanAmbassadorHeadshot())
					return HitscanOffset::HEAD;

				return HitscanOffset::CHEST;
			}

			case TF_WEAPON_SNIPERRIFLE:
			case TF_WEAPON_SNIPERRIFLE_DECAP:
			case TF_WEAPON_SNIPERRIFLE_CLASSIC:
			{
				if (!pLocal->InCond(TF_COND_ZOOMED))
					return HitscanOffset::CHEST;

				if (static_cast<CTFSniperRifle*>(pWeapon)->m_flChargedDamage() < 50.0f)
					return HitscanOffset::CHEST;

				return HitscanOffset::HEAD;
			}

		}
		
		return HitscanOffset::CHEST;
	}

	bool GetShotPosition(CTFPlayer* pLocal, CBaseEntity* pTarget, CTFWeaponBase* pWeapon, Vector eyePos, Vector& shotPosition)
	{
		matrix3x4 bones[MAXSTUDIOBONES];
		if (!pTarget->SetupBones(bones, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, interfaces::GlobalVars->curtime))
			return false;

		CGameTrace trace;
		CTraceFilterHitscan filter;
		filter.pSkip = pLocal;

		auto initialOffset = GetInitialOffset(pLocal, pWeapon);
		switch(initialOffset)
		{
			case HitscanOffset::HEAD:
			{
				Vector boneCenter;
				static_cast<CBaseAnimating*>(pTarget)->GetHitboxCenter(bones, HITBOX_HEAD, boneCenter);

				helper::engine::Trace(eyePos, boneCenter, MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);
				if (!trace.DidHit() || trace.m_pEnt != pTarget)
					break;

				shotPosition = boneCenter;
				return true;
			}
			case HitscanOffset::CHEST:
			{
				Vector boneCenter;
				static_cast<CBaseAnimating*>(pTarget)->GetHitboxCenter(bones, HITBOX_SPINE3, boneCenter);

				helper::engine::Trace(eyePos, boneCenter, MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);
				if (!trace.DidHit() || trace.m_pEnt != pTarget)
					break;

				shotPosition = boneCenter;
				return true;
			}
                }

                for (int i = 0; i < HITBOX_LEFT_UPPERARM; i++)
		{
			Vector bonePos;
			if (!static_cast<CBaseAnimating*>(pTarget)->GetHitboxCenter(bones, i, bonePos))
				continue;

			helper::engine::Trace(eyePos, bonePos, MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);
			if (!trace.DidHit() || trace.m_pEnt != pTarget)
				continue;

			shotPosition = bonePos;
			return true;
		}

		helper::engine::Trace(eyePos, pTarget->GetCenter(), MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);
		if (!trace.DidHit() || trace.m_pEnt != pTarget)
			return false;

		shotPosition = pTarget->GetCenter();
		return true;
	}

	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, AimbotState& state)
	{
		if (settings.aimbot.waitforcharge && pWeapon->IsAmbassador())
			if (!pWeapon->CanAmbassadorHeadshot())
				return;

		std::vector<PotentialTarget> targets;

		int localTeam = pLocal->m_iTeamNum();
		Vector shootPos = pLocal->GetEyePos();

		Vector viewAngles;
		interfaces::Engine->GetViewAngles(viewAngles);

		Vector viewForward;
		Math::AngleVectors(viewAngles, &viewForward);
		viewForward.Normalize();

		float fovRad = DEG2RAD(settings.aimbot.fov);
		float minDot = cosf(fovRad);

		CGameTrace trace;
		CTraceFilterHitscan filter;
		filter.pSkip = pLocal;

		bool bIsSniperRifle = pWeapon->IsSniperRifle();
		bool bIsZoomed = pLocal->InCond(TF_COND_ZOOMED);

		auto scanList = [&](const auto& list)
		{
			for (CBaseEntity* entity : list)
			{
				if (!AimbotUtils::IsValidEntity(entity, 0))
					continue;
	
				Vector pos;
				{
					if (entity->IsPlayer())
						{
							if (!GetShotPosition(pLocal, entity, pWeapon, shootPos, pos))
								continue;
						}
					else if (entity->IsBuilding())
						pos = reinterpret_cast<CBaseObject*>(entity)->GetCenter();
					else
						pos = entity->GetAbsOrigin();
				}
	
				Vector dir = pos - shootPos;
				float distance = dir.Normalize();
	
				if (distance >= 2048.f)
					continue;
	
				float dot = dir.Dot(viewForward);
	
				if (dot < minDot)
					continue;
	
				if (settings.aimbot.waitforcharge && bIsZoomed && bIsSniperRifle && !AimbotUtils::CanDamageWithSniperRifle(pLocal, entity, pWeapon))
					continue;
	
				// GetShotPosition already checks if its visible
				if (!entity->IsPlayer())
				{
					helper::engine::Trace(shootPos, pos, MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);
					if (!trace.DidHit() || trace.m_pEnt != entity)
						continue;
				}
	
				targets.emplace_back(PotentialTarget{dir, pos, distance, dot, entity});
			}
		};

		{
			bool bCanHitTeammates = pWeapon->CanHitTeammates();
			TeamMode teamMode = settings.aimbot.teamMode;

			if (!bCanHitTeammates || teamMode == TeamMode::ONLYENEMY || teamMode == TeamMode::BOTH)
				scanList(EntityList::GetEnemies());

			if (bCanHitTeammates && (teamMode == TeamMode::ONLYTEAMMATE || teamMode == TeamMode::BOTH))
				scanList(EntityList::GetTeammates());
		}

		if (targets.empty())
			return;

		AimbotUtils::QuickSort(targets, 0, targets.size() - 1);
		AimbotMode mode = settings.aimbot.mode;

		switch(mode)
		{
			case AimbotMode::PLAIN:
			{
				if (settings.aimbot.autoshoot)
					pCmd->buttons |= IN_ATTACK;

				auto target = targets.front();
				Vector angle = target.dir.ToAngle();

				pCmd->viewangles = angle;
				interfaces::Engine->SetViewAngles(angle);
				break;
			}
			case AimbotMode::SMOOTH:
			{
				auto target = targets.front();
				Vector targetAngle = target.dir.ToAngle();
				
				Vector delta = targetAngle - viewAngles;
				Vector smoothedAngle = viewAngles + (delta * settings.aimbot.smoothness * 0.01f);
				state.angle = smoothedAngle;

				interfaces::Engine->SetViewAngles(smoothedAngle);
				pCmd->viewangles = smoothedAngle;
				
				state.running = true;

				CGameTrace trace;
				CTraceFilterHitscan filter;
				filter.pSkip = pLocal;
				helper::engine::Trace(shootPos, shootPos + (viewForward * 2048), 
							MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);

				if (!trace.DidHit() || trace.m_pEnt != target.entity)
					break;

				if (settings.aimbot.autoshoot)
					pCmd->buttons |= IN_ATTACK;
				break;
			}
			case AimbotMode::ASSISTANCE:
			{
				// not moving the mouse, dont do anything
				if (pCmd->mousedx == 0 && pCmd->mousedy == 0)
					break;
				
				auto target = targets.front();
				Vector targetAngle = target.dir.ToAngle();

				Vector delta = targetAngle - viewAngles;
				Vector smoothedAngle = viewAngles + (delta * settings.aimbot.smoothness * 0.01f);

				interfaces::Engine->SetViewAngles(smoothedAngle);
				pCmd->viewangles = smoothedAngle;
				state.angle = smoothedAngle;
				
				state.running = true;

				CGameTrace trace;
				CTraceFilterHitscan filter;
				filter.pSkip = pLocal;
				helper::engine::Trace(shootPos, shootPos + (viewForward * 2048), 
							MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);

				if (!trace.DidHit() || trace.m_pEnt != target.entity)
					break;

				if (settings.aimbot.autoshoot)
					pCmd->buttons |= IN_ATTACK;
				break;
			}
			case AimbotMode::SILENT:
			case AimbotMode::PSILENT:
			{
				if (settings.aimbot.autoshoot)
					pCmd->buttons |= IN_ATTACK;

				if (helper::localplayer::IsAttacking(pLocal, pWeapon, pCmd))
				{
					auto target = targets.front();
					Vector angle = target.dir.ToAngle();
					pCmd->viewangles = angle;
					state.angle = angle;
					state.running = true;
				}

				break;
			}

                        case AimbotMode::INVALID:
                        case AimbotMode::MAX:
                        	break;
                        }

                if (targets.front().entity != nullptr)
			EntityList::m_pAimbotTarget = targets.front().entity;
	}
};