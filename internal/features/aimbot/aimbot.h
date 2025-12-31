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
		if (!settings.aimbot.enabled)
			return;

		if (!interfaces::inputsystem->IsButtonDown(KEY_LSHIFT))
			return;

		//interfaces::vstdlib->ConsolePrintf("slot: %d\n", pWeapon->GetWeaponInfo()->iSlot);
		//interfaces::vstdlib->ConsolePrintf("Type: %d\n", pWeapon->GetWeaponType());
		//interfaces::vstdlib->ConsolePrintf("Damage type: %d\n", pWeapon->GetProjectileType());

		switch (pWeapon->GetWeaponType())
		{
			case EWeaponType::HITSCAN:
				{
					static AimbotHitscan hitscan;
					hitscan.Run(pLocal, pWeapon, pCmd);
					return;
				}
				break;
			case EWeaponType::MELEE:
			case EWeaponType::PROJECTILE:
			default: return;
		}
	}
};