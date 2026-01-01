#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../features/aimbot/aimbot.h"
#include <string>

using CreateMoveFn = bool (*)(IClientMode* thisptr, float sample_frametime, CUserCmd* pCmd);
inline CreateMoveFn originalCreateMove = nullptr;

inline bool HookedCreateMove (IClientMode* thisptr, float sample_frametime, CUserCmd* pCmd)
{
	if (!pCmd || !pCmd->command_number)
		return originalCreateMove(thisptr, sample_frametime, pCmd);

	if (!interfaces::engine->IsInGame() || !interfaces::engine->IsConnected())
		return originalCreateMove(thisptr, sample_frametime, pCmd);
	
	Vector originalAngles = pCmd->viewangles;

	// populate movement
	int ret = originalCreateMove(thisptr, sample_frametime, pCmd);
	//bool* bSendPacket = (bool*)((uintptr_t)__builtin_return_address(0) + 0x20); ?? is it really this?

	CTFPlayer* pLocal = helper::engine::GetLocalPlayer();
	if (!pLocal || !pLocal->IsAlive())
		return ret;

	CTFWeaponBase* pWeapon = HandleAs<CTFWeaponBase>(pLocal->GetActiveWeapon());
	if (!pWeapon)
		return ret;

	Aimbot::Run(pLocal, pWeapon, pCmd);
	helper::engine::FixMovement(pCmd, originalAngles, pCmd->viewangles);

	// Return false so the engine doesn't apply it to engine->SetViewAngles; (this is stupid)*/
	return false;
}

inline void HookCreateMove()
{
	void** vt = vtable::get(interfaces::clientMode);
	originalCreateMove = vtable::hook(vt, 22, &HookedCreateMove);

	helper::console::ColoredPrint("ClientModeShared::CreateMove hooked\n", (Color_t){100, 255, 100, 255});
}