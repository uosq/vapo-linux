#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"
#include <string>

#include "../features/aimbot/aimbot.h"

#include "../libdetour/libdetour.h"

using CalcViewModelViewFn = void(*)(void* thisptr, CTFPlayer* owner, const Vector& eyePosition, const QAngle& eyeAngles);
inline CalcViewModelViewFn originalCalcViewModelView = nullptr;

inline detour_ctx_t ctx;

DETOUR_DECL_TYPE(void, originalCalcViewModelView, void* thisptr, CTFPlayer*, const Vector&, const Vector&);

inline float stoptime = 0.0f;
#define VIEWMODELAIM_INTERVAL 0.5f

inline void HookedCalcViewModelView(void* thisptr, CTFPlayer* owner, const Vector& eyePosition, const Vector& eyeAngles)
{
	if (helper::engine::IsTakingScreenshot())
	{
		DETOUR_ORIG_CALL(&ctx, originalCalcViewModelView, thisptr, owner, eyePosition, eyeAngles);
		return;
	}

	if (settings.aimbot.viewmodelaim && Aimbot::IsRunning())
		stoptime = globalvars->curtime + VIEWMODELAIM_INTERVAL;

	if (settings.aimbot.viewmodelaim && (globalvars->curtime < stoptime))
	{
		DETOUR_ORIG_CALL(&ctx, originalCalcViewModelView, thisptr, owner, eyePosition, Aimbot::GetAngle());
		return;
	}

	//originalCalcViewModelView(thisptr, owner, eyePosition, Vector(0, 0, 0));
	DETOUR_ORIG_CALL(&ctx, originalCalcViewModelView, thisptr, owner, eyePosition, eyeAngles);
}

inline void HookCalcViewModelView()
{
	originalCalcViewModelView = (CalcViewModelViewFn)(sigscan_module("client.so", "55 48 89 E5 41 57 41 56 41 55 49 89 F5 41 54 49 89 FC 53 48 83 EC 48 8B 41 08 48 8B 31 89 45 A8 89 45 B4 48 8B 02 48 89 75 A0 48 89 75 AC 48 89 45 B8 8B 42 08 8B 97 4C 0C"));
	detour_init(&ctx, (void*)originalCalcViewModelView, (void*)&HookedCalcViewModelView);
	detour_enable(&ctx);
	interfaces::vstdlib->ConsolePrintf("CalcViewModelView hooked\n");
}