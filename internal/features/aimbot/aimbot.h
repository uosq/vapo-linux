#pragma once

#include "../../sdk/definitions/cusercmd.h"
#include "../../sdk/classes/player.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/handle_utils.h"

#include "hitscan/aimbot_hitscan.h"
#include "projectile/aimbot_projectile.h"

namespace Aimbot
{
	static inline std::vector<Vector> targetPath;
	static inline Vector angle{0, 0, 0};
	static bool running = true;
	static bool shouldSilent = false;

	inline Vector GetAngle()
	{
		return angle;
	}

	inline bool IsRunning()
	{
		return running;
	}

	inline bool ShouldSilent()
	{
		return shouldSilent;
	}

	inline void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, bool* bSendPacket)
	{
		running = false;
		shouldSilent = false;

		if (!settings.aimbot.enabled)
			return;

		if (helper::engine::IsConsoleVisible() || helper::engine::IsGameUIVisible() || helper::engine::IsTakingScreenshot())
			return;

		ButtonCode_t key = interfaces::InputSystem->StringToButtonCode(settings.aimbot.key.c_str());

		if (key && !interfaces::InputSystem->IsButtonDown(key))
			return;

		switch (pWeapon->GetWeaponType())
		{
			case EWeaponType::HITSCAN:
			{
				static AimbotHitscan hitscan;
				hitscan.Run(pLocal, pWeapon, pCmd, angle, running);
				break;
			} break;
			case EWeaponType::PROJECTILE:
			{
				static AimbotProjectile projectile;
				projectile.Run(pLocal, pWeapon, pCmd, targetPath, angle, running, shouldSilent);

				if (shouldSilent)
					*bSendPacket = false;
				break;
			} break;

			case EWeaponType::MELEE:
			default: break;
		}
	}

	inline void DrawFOVIndicator(CTFPlayer* pLocal, CTFWeaponBase* pWeapon)
	{
		if (settings.aimbot.fov >= 90)
			return;

		float aimFov = DEG2RAD(settings.aimbot.fov);
		float camFov = DEG2RAD(pLocal->m_iDefaultFOV() * 0.5f);

		int w, h;
		helper::draw::GetScreenSize(w, h);

		float radius = tan(aimFov)/tan(camFov) * w*0.5f * ((float)3/(float)4);

		helper::draw::SetColor(255, 255, 255, 255);
		interfaces::Surface->DrawOutlinedCircle((int)(w*0.5f), (int)(h*0.5f), (int)(radius), 64);
	}

	inline void CleanTargetPath()
	{
		static float lastcleartime = 0.0f;
		if (targetPath.empty())
			return;

		if (interfaces::GlobalVars && (interfaces::GlobalVars->realtime - lastcleartime) < 5.0f)
			return;

		targetPath.clear();
		lastcleartime = interfaces::GlobalVars->realtime;
	}

	inline void DrawTargetPath()
	{
		//CleanTargetPath();

		if (targetPath.size() < 2)
			return;

		Vector prevScreen;
		if (!helper::engine::WorldToScreen(targetPath[0], prevScreen))
			return;

		helper::draw::SetColor({255, 255, 255, 255});

		for (size_t i = 1; i < targetPath.size(); i++)
		{
			Vector currScreen;
			if (!helper::engine::WorldToScreen(targetPath[i], currScreen))
				continue;

			interfaces::Surface->DrawLine(prevScreen.x, prevScreen.y, currScreen.x, currScreen.y);
			prevScreen = currScreen;
		}
	}
};