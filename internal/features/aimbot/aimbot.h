#pragma once

#include "../../sdk/definitions/cusercmd.h"
#include "../../sdk/classes/player.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/handle_utils.h"

#include "hitscan/aimbot_hitscan.h"
#include "projectile/aimbot_projectile.h"

struct Aimbot
{
	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
	{
		if (!settings.aimbot.enabled)
			return;

		ButtonCode_t key = interfaces::inputsystem->StringToButtonCode(settings.aimbot.key.c_str());

		if (key && !interfaces::inputsystem->IsButtonDown(key))
			return;

		switch (pWeapon->GetWeaponType())
		{
			case EWeaponType::HITSCAN:
			{
				static AimbotHitscan hitscan;
				hitscan.Run(pLocal, pWeapon, pCmd);
				return;
			} break;
			case EWeaponType::PROJECTILE:
			{
				static AimbotProjectile projectile;
				projectile.Run(pLocal, pWeapon, pCmd);
				return;
			} break;

			case EWeaponType::MELEE:
			default: return;
		}
	}
};