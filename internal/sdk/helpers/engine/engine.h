#pragma once

#include "../../interfaces/interfaces.h"
#include "../../definitions/color.h"
#include "../../classes/player.h"
#include "../../classes/weaponbase.h"
#include "../math.h"

namespace helper
{
	namespace engine
	{
		inline bool IsGameUIVisible()
		{
			return interfaces::EngineVGui->IsGameUIVisible();
		}

		inline int GetMaxClients()
		{
			return interfaces::Engine->GetMaxClients();
		}

		inline bool IsTakingScreenshot()
		{
			return interfaces::Engine->IsTakingScreenshot();
		}

		inline bool IsConsoleVisible()
		{
			return interfaces::Engine->Con_IsVisible();
		}

		inline CTFPlayer* GetLocalPlayer()
		{
			int index = interfaces::Engine->GetLocalPlayer();
			return (CTFPlayer*)interfaces::EntityList->GetClientEntity(index);
		}

		inline void FixMovement(CUserCmd* pCmd, const Vec3& vCurAngle, const Vec3& vTargetAngle)
		{
			bool bCurOOB = fabsf(Math::NormalizeAngle(vCurAngle.x)) > 90.f;
			bool bTargetOOB = fabsf(Math::NormalizeAngle(vTargetAngle.x)) > 90.f;
		
			Vec3 vMove = { pCmd->forwardmove, pCmd->sidemove * (bCurOOB ? -1 : 1), pCmd->upmove};
			float flSpeed = vMove.Length2D();
			Vec3 vMoveAng = Math::VectorAngles(vMove);

			float flCurYaw = vCurAngle.y + (bCurOOB ? 180.f : 0.f);
			float flTargetYaw = vTargetAngle.y + (bTargetOOB ? 180.f : 0.f);
			float flYaw = DEG2RAD(flTargetYaw - flCurYaw + vMoveAng.y);

			pCmd->forwardmove = cos(flYaw) * flSpeed;
			pCmd->sidemove = sin(flYaw) * flSpeed * (bTargetOOB ? -1 : 1);
		}

		inline void FixMovement(CUserCmd* pCmd, const Vec3& vTargetAngle)
		{
			FixMovement(pCmd, pCmd->viewangles, vTargetAngle);
		}

		inline bool GetViewMatrix(VMatrix &out)
		{
			CViewSetup view;
			if (interfaces::BaseClientDLL->GetPlayerView(view))
			{
				VMatrix mWorldToView, mViewToProjection, mWorldToPixels;
				interfaces::RenderView->GetMatricesForView(view, &mWorldToView, &mViewToProjection, &out, &mWorldToPixels);
				return true;
			}
			return false;
		}

		// pasted from amalgam
		// i was gonna use cazz's tutorial
		// but his viewmatrix doesn't make sense to me
		inline bool WorldToScreen(const Vector& pos, Vector& screenPosOut)
		{
			VMatrix vm;
			if (!GetViewMatrix(vm))
				return false;

			float flW = vm[3][0] * pos.x + vm[3][1] * pos.y + vm[3][2] * pos.z + vm[3][3];
			if (flW < 0.001f)
				return false;

			int screenW, screenH;
			interfaces::Engine->GetScreenSize(screenW, screenH);
				
			const float fl1dbw = 1 / flW;

			screenPosOut.x = (screenW / 2.0f) + ((vm[0][0] * pos.x + vm[0][1] * pos.y + vm[0][2] * pos.z + vm[0][3]) * fl1dbw) * screenW / 2.0f + 0.5f;
			screenPosOut.y = (screenH / 2.0f) - ((vm[1][0] * pos.x + vm[1][1] * pos.y + vm[1][2] * pos.z + vm[1][3]) * fl1dbw) * screenH / 2.0f + 0.5f;

			return true;
		}

		inline float AttributeHookValue(float defaultValue, const char *attrib_name, CBaseEntity *entity)
		{
			return interfaces::attributeManager.AttributeHookValue(defaultValue, attrib_name, entity);
		}

		inline void Trace(Vector start, Vector end, unsigned int mask, ITraceFilter* pFilter, CGameTrace* pTrace)
		{
			Ray_t ray;
			ray.Init(start, end);
			interfaces::EngineTrace->TraceRay(ray, mask, pFilter, pTrace);
		}

		inline void TraceHull(Vector start, Vector end, Vector hullmin, Vector hullmax, unsigned mask, ITraceFilter* pFilter, CGameTrace* pTrace)
		{
			Ray_t ray;
			ray.Init(start, end, hullmin, hullmax);
			interfaces::EngineTrace->TraceRay(ray, mask, pFilter, pTrace);
		}

		inline bool IsInMatch()
		{
			return interfaces::Engine->IsInGame() && interfaces::Engine->IsConnected();
		}
	}
};