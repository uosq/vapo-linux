#pragma once

#include <string>
#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../features/aimbot/aimbot.h"
#include "../features/bhop/bhop.h"
#include "../features/triggerbot/triggerbot.h"
#include "../features/entitylist/entitylist.h"
#include "../features/antiaim/antiaim.h"

// Source https://8dcc.github.io/reversing/reversing-tf2-bsendpacket.html#introduction
#define SENDPACKET_STACK_OFFSET 0xF8

DECLARE_VTABLE_HOOK(CreateMove, bool, (IClientMode* thisptr, float sample_frametime, CUserCmd* pCmd))
{
	bool ret = originalCreateMove(thisptr, sample_frametime, pCmd);

	if (!pCmd || !pCmd->command_number)
		return ret;

	if (!interfaces::Engine->IsInGame() || !interfaces::Engine->IsConnected())
		return ret;

	CTFPlayer* pLocal = EntityList::GetLocal();
	if (!pLocal || !pLocal->IsAlive() || pLocal->IsTaunting())
		return ret;

	CTFWeaponBase* pWeapon = HandleAs<CTFWeaponBase*>(pLocal->GetActiveWeapon());
	if (pWeapon == nullptr)
		return ret;

	Vector originalAngles = pCmd->viewangles;

	uintptr_t current_frame_address = reinterpret_cast<uintptr_t>(__builtin_frame_address(0));
    	uintptr_t current_stack_address = current_frame_address + 0x8;
    	bool* bSendPacket = (bool*)(current_stack_address + SENDPACKET_STACK_OFFSET);

	Bhop::Run(pLocal, pCmd);
	Antiaim::Run(pLocal, pWeapon, pCmd, bSendPacket);
	Aimbot::Run(pLocal, pWeapon, pCmd, bSendPacket);
	Triggerbot::Run(pLocal, pWeapon, pCmd);

	if (*bSendPacket == true)
		helper::localplayer::LastAngle = pCmd->viewangles;

	helper::engine::FixMovement(pCmd, originalAngles, pCmd->viewangles);
	// Return false so the engine doesn't apply it to engine->SetViewAngles; (this is stupid)
	return false;
}

inline void HookCreateMove()
{
	INSTALL_VTABLE_HOOK(CreateMove, interfaces::ClientMode, 22);
	helper::console::ColoredPrint("ClientModeShared::CreateMove hooked\n", (Color_t){100, 255, 100, 255});
}