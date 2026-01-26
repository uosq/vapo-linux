#pragma once

#include "../../classes/player.h"
#include "../../classes/weaponbase.h"
#include "../../definitions/hudbasechat.h"
#include <cstdarg>

namespace helper
{
	namespace localplayer
	{
		inline bool CanShoot(CTFPlayer *pLocal, CTFWeaponBase *pWeapon, CUserCmd* pCmd, bool ignoreAttack = false)
		{
			if (!pLocal || !pWeapon)
				return false;

			return (ignoreAttack || (pCmd->buttons & IN_ATTACK)) && pWeapon->CanPrimaryAttack();
		}

		inline bool IsAttacking(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd)
		{
			if (pLocal == nullptr || pWeapon == nullptr)
				return false;

			switch (pWeapon->GetWeaponType())
			{
				case EWeaponType::HITSCAN:
				{
					int id = pWeapon->GetWeaponID();
					if (id == TF_WEAPON_SNIPERRIFLE_CLASSIC)
						return pWeapon->CanPrimaryAttack() && !(pCmd->buttons & IN_ATTACK) && static_cast<CTFSniperRifleClassic*>(pWeapon)->m_flChargedDamage() > 0.0f;

					if (id == TF_WEAPON_MINIGUN)
					{
						int state = static_cast<CTFMinigun*>(pWeapon)->m_iWeaponState();
						if (state == AC_STATE_FIRING || state == AC_STATE_SPINNING)
							return pWeapon->CanPrimaryAttack() && pWeapon->HasPrimaryAmmoForShot() && (pCmd->buttons & IN_ATTACK);
						return false;
					}

					return pWeapon->HasPrimaryAmmoForShot() && CanShoot(pLocal, pWeapon, pCmd);
				}

				case EWeaponType::PROJECTILE:
				{
					int id = pWeapon->GetWeaponID();
					if (id == TF_WEAPON_COMPOUND_BOW || id == TF_WEAPON_PIPEBOMBLAUNCHER)
					{
						float flchargebegintime = static_cast<CTFPipebombLauncher*>(pWeapon)->m_flChargeBeginTime();
						float charge = flchargebegintime > 0.f ? TICKS_TO_TIME(pLocal->GetTickBase()) - flchargebegintime : 0.f;
						return charge > 0.0f && !(pCmd->buttons & IN_ATTACK);
					}

					if (pWeapon->m_iItemDefinitionIndex() == Soldier_m_TheBeggarsBazooka)
						return pWeapon->CanPrimaryAttack() && pWeapon->m_iClip1() > 0 && !(pCmd->buttons & IN_ATTACK);

					if (id == TF_WEAPON_CANNON)
					{
						float mortar = AttributeHookValue(0.0f, "grenade_launcher_mortar_mode", pWeapon, nullptr, true);
						if (mortar == 0.0f)
							return CanShoot(pLocal, pWeapon, pCmd);

						float detonatetime = static_cast<CTFGrenadeLauncher*>(pWeapon)->m_flDetonateTime();
						float charge = detonatetime > 0.0f ? mortar - detonatetime - TICKS_TO_TIME(pLocal->GetTickBase()) : 0.0f;
						charge = Math::RemapVal(charge, 0.0f, mortar, 0.0f, 1.0f);
						return charge == 1.0f || (!(pCmd->buttons & IN_ATTACK) && charge > 0.0f);
					}

					return CanShoot(pLocal, pWeapon, pCmd);
				}

				case EWeaponType::MELEE:
				{
					int id = pWeapon->GetWeaponID();
					if (id == TF_WEAPON_KNIFE)
						return CanShoot(pLocal, pWeapon, pCmd);
					
					return pWeapon->m_flSmackTime() != -1.0f && interfaces::GlobalVars->curtime >= pWeapon->m_flSmackTime();
				}

				case EWeaponType::UNKNOWN:
				break;
                        }

			return false;
		}

		inline Vector LastAngle{};

		inline void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt)
		{
			// xref: Console
			using ChatPrintfFn = void(*)(CBaseHudChat*, int, int, const char*);
			static ChatPrintfFn original = reinterpret_cast<ChatPrintfFn>(sigscan_module("client.so", "55 48 89 E5 41 57 41 56 41 55 49 89 FD 41 54 41 89 F4 53 89 D3 48 89 CA"));
			original(interfaces::gHUD, iPlayerIndex, iFilter, fmt);
		}
	}
}