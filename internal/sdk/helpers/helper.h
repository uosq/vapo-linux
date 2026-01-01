#pragma once

#include "../interfaces/interfaces.h"
#include "../definitions/color.h"
#include "../classes/player.h"
#include "fonts.h"
#include "../math.h"

#include <cmath>
#include <cstring>
#include <string>

namespace helper
{
	namespace draw
	{

		inline HFont CreateFont(const char* name, int height, int weight, int flags = 0)
		{
			return fontManager.CreateFont(name, height, weight, flags);
		}

		inline void SetFont(HFont font)
		{
			fontManager.SetCurrentFont(font);
			interfaces::surface->DrawSetTextFont(font);
		}

		inline HFont GetCurrentFont()
		{
			return fontManager.GetCurrentFont();
		}

		inline void FilledRect(int x0, int y0, int x1, int y1)
		{
			interfaces::surface->DrawFilledRect(x0, y0, x1, y1);
		}

		inline void OutlinedRect(int x0, int y0, int x1, int y1)
		{
			interfaces::surface->DrawOutlinedRect(x0, y0, x1, y1);
		}

		inline void SetColor(Color col)
		{
			interfaces::surface->DrawSetColor(col);
		}

		inline void SetColor(int r, int g, int b, int a = 255)
		{
			interfaces::surface->DrawSetColor(r, g, b, a);
		}

		inline void Text(int x, int y, Color color, std::string text)
		{
			// convert our string to wide string
			// fuck my life
			std::wstring wtext(text.begin(), text.end());
			interfaces::surface->DrawSetTextPos(x, y);
			interfaces::surface->DrawSetTextColor(color);
			interfaces::surface->DrawPrintText(wtext.c_str(), wtext.length());
		}

		inline void GetMousePosition(int &x, int &y)
		{
			interfaces::surface->SurfaceGetCursorPos(x, y);
		}

		inline void GetTextSize(std::string text, int &width, int &height)
		{
			std::wstring wtext(text.begin(), text.end());
			interfaces::surface->GetTextSize(fontManager.GetCurrentFont(), wtext.c_str(), width, height);
		}

		inline std::wstring ConvertStringToWChar(std::string text)
		{
			return std::wstring(text.begin(), text.end());
		}

		inline void GetScreenSize(int &w, int &h)
		{
			w = interfaces::baseClientDll->GetScreenWidth();
			h = interfaces::baseClientDll->GetScreenHeight();
		}
	};

	namespace engine
	{
		inline bool IsGameUIVisible()
		{
			return interfaces::enginevgui->IsGameUIVisible();
		}

		inline int GetMaxClients()
		{
			return interfaces::enginetool->GetMaxClients();
		}

		inline bool IsTakingScreenshot()
		{
			return interfaces::engine->IsTakingScreenshot();
		}

		inline bool IsConsoleVisible()
		{
			return interfaces::engine->Con_IsVisible();
		}

		inline CTFPlayer* GetLocalPlayer()
		{
			int index = interfaces::engine->GetLocalPlayer();
			return (CTFPlayer*)interfaces::entitylist->GetClientEntity(index);
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
			if (interfaces::baseClientDll->GetPlayerView(view))
			{
				VMatrix mWorldToView, mViewToProjection, mWorldToPixels;
				interfaces::renderview->GetMatricesForView(view, &mWorldToView, &mViewToProjection, &out, &mWorldToPixels);
				return true;
			}

			return false;
		}

		// pasted from amalgam
		// i was gonna use cazz's tutorial
		// but his viewmatrix doesn't make sense to me
		inline bool WorldToScreen(const Vector& pos, Vector& screen)
		{
			VMatrix vm;
			if (!GetViewMatrix(vm))
				return false;

			float flW = vm[3][0] * pos.x + vm[3][1] * pos.y + vm[3][2] * pos.z + vm[3][3];
			bool bOnScreen = flW > 0.f;

			if (bOnScreen)
			{
				const float fl1dbw = 1 / fabs(flW);
				int screenW, screenH;
				interfaces::engine->GetScreenSize(screenW, screenH);

				screen.x = (screenW / 2.f) + ((vm[0][0] * pos.x + vm[0][1] * pos.y + vm[0][2] * pos.z + vm[0][3]) * fl1dbw) * screenW / 2 + 0.5f;
				screen.y = (screenH / 2.f) - ((vm[1][0] * pos.x + vm[1][1] * pos.y + vm[1][2] * pos.z + vm[1][3]) * fl1dbw) * screenH / 2 + 0.5f;
			}

			return bOnScreen;
		}

		inline float AttributeHookValue(float defaultValue, const char *attrib_name, CBaseEntity *entity)
		{
			return interfaces::attributeManager.AttributeHookValue(defaultValue, attrib_name, entity);
		}

		inline void Trace(Vector start, Vector end, unsigned int mask, ITraceFilter* pFilter, CGameTrace* pTrace)
		{
			Ray_t ray;
			ray.Init(start, end);
			interfaces::enginetrace->TraceRay(ray, mask, pFilter, pTrace);
		}

		inline void TraceHull(Vector start, Vector end, Vector hullmin, Vector hullmax, unsigned mask, ITraceFilter* pFilter, CGameTrace* pTrace)
		{
			Ray_t ray;
			ray.Init(start, end, hullmin, hullmax);
			interfaces::enginetrace->TraceRay(ray, mask, pFilter, pTrace);
		}
	};

	namespace console
	{
		inline void Print(const char* text)
		{
			interfaces::vstdlib->ConsolePrintf(text);
		}

		inline void ColoredPrint(const char* text, Color_t color)
		{
			interfaces::vstdlib->ConsoleColorPrintf(color, text);
		}
	};
};