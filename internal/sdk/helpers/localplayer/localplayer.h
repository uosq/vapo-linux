#pragma once

#include "../../classes/player.h"
#include "../../classes/weaponbase.h"

namespace helper
{
	namespace localplayer
	{
		inline bool CanShoot(CTFPlayer *pLocal, CTFWeaponBase *pWeapon)
		{
			if (!pLocal || !pWeapon)
				return false;
			return pWeapon->CanPrimaryAttack() || (pWeapon->IsInReload() && pWeapon->m_iClip1() > 0);
		}

		inline Vector LastAngle{};
	}
}