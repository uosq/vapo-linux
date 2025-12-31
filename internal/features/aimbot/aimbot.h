#pragma once

#include "../../sdk/definitions/cusercmd.h"
#include "../../sdk/classes/player.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/handle_utils.h"
#include "hitscan/aimbot_hitscan.h"

struct Aimbot
{
	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
	{
		interfaces::vstdlib->ConsolePrintf("id: %d\n", pWeapon->GetWeaponID());
		//interfaces::vstdlib->ConsolePrintf("Type: %d\n", pWeapon->GetWeaponType());
		//interfaces::vstdlib->ConsolePrintf("Damage type: %d\n", pWeapon->GetProjectileType());
	}
};