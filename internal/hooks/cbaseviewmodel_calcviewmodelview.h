#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"
#include <string>

#include "../features/aimbot/aimbot.h"

#include "../libdetour/libdetour.h"

using CalcViewModelViewFn = void(*)(void* thisptr, CBaseEntity* owner, const Vector& eyePosition, const QAngle& eyeAngles);
inline CalcViewModelViewFn originalCalcViewModelView = nullptr;

inline detour_ctx_t calcViewModel_ctx;

DETOUR_DECL_TYPE(void, originalCalcViewModelView, void* thisptr, CBaseEntity*, const Vector&, const QAngle&);
#define VIEWMODELAIM_INTERVAL 0.5f
inline static float stoptime = 0.0f;

inline void HookedCalcViewModelView(void* thisptr, CBaseEntity* owner, const Vector& eyePosition, const QAngle& eyeAngles)
{
	Vector angle = eyeAngles;

	if (owner && settings.aimbot.viewmodelaim)
	{	// viewmodel aim
		if (Aimbot::IsRunning() && interfaces::GlobalVars && interfaces::GlobalVars->curtime)
			stoptime = interfaces::GlobalVars->curtime + VIEWMODELAIM_INTERVAL;
	
		if (interfaces::GlobalVars && interfaces::GlobalVars->curtime && interfaces::GlobalVars->curtime < stoptime)
			angle = Aimbot::GetAngle();
	}

	DETOUR_ORIG_CALL(&calcViewModel_ctx, originalCalcViewModelView, thisptr, owner, eyePosition, angle);
}

inline void HookCalcViewModelView()
{
	originalCalcViewModelView = (CalcViewModelViewFn)(sigscan_module("client.so", "55 48 89 E5 41 57 41 56 41 55 49 89 F5 41 54 49 89 FC 53 48 83 EC 48 8B 41 08 48 8B 31 89 45 A8 89 45 B4 48 8B 02 48 89 75 A0 48 89 75 AC 48 89 45 B8 8B 42 08 8B 97 4C 0C"));
	detour_init(&calcViewModel_ctx, (void*)originalCalcViewModelView, (void*)&HookedCalcViewModelView);
	detour_enable(&calcViewModel_ctx);

	Color_t color{100, 255, 100, 255};
	interfaces::Cvar->ConsoleColorPrintf(color, "CalcViewModelView hooked\n");
}