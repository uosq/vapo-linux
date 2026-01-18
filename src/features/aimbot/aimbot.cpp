#include "aimbot.h"
#include "melee/aimbot_melee.h"

namespace Aimbot
{
	AimbotState state = {};

	Vector GetAngle()
	{
		return state.angle;
	}

	bool IsRunning()
	{
		return state.running;
	}

	bool ShouldSilent()
	{
		return state.shouldSilent;
	}

	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, bool* bSendPacket)
	{
		ClearAimbotState(state);

		if (!settings.aimbot.enabled)
			return;

		if (helper::engine::IsConsoleVisible() || helper::engine::IsGameUIVisible() || helper::engine::IsTakingScreenshot())
			return;

		ButtonCode_t key = interfaces::InputSystem->StringToButtonCode(settings.aimbot.key.c_str());

		if (key != BUTTON_CODE_INVALID && !interfaces::InputSystem->IsButtonDown(key))
			return;

		switch (pWeapon->GetWeaponType())
		{
			case EWeaponType::HITSCAN:
			{
				static AimbotHitscan hitscan;
				hitscan.Run(pLocal, pWeapon, pCmd, state);
			} break;
			
			case EWeaponType::PROJECTILE:
			{
				static AimbotProjectile projectile;
				projectile.Run(pLocal, pWeapon, pCmd, state);

				if (settings.aimbot.psilent && state.shouldSilent)
					*bSendPacket = false;
			} break;

			case EWeaponType::MELEE:
			{
				static AimbotMelee melee;
				melee.Run(pLocal, pWeapon, pCmd, state);

				if (settings.aimbot.psilent && state.shouldSilent)
					*bSendPacket = false;
			} break;
			default: break;
		}
	}

	void DrawFOVIndicator(CTFPlayer* pLocal)
	{
		if (settings.aimbot.fov >= 90 || !settings.aimbot.draw_fov_indicator || helper::engine::IsTakingScreenshot())
			return;

		float aimFov = DEG2RAD(settings.aimbot.fov);
		float camFov = 0.0f;
		if (settings.misc.customfov_enabled)
			camFov = DEG2RAD(std::max(settings.misc.customfov * 0.5f, 1.0f));
		else
			camFov = DEG2RAD(pLocal->m_iDefaultFOV() * 0.5f);

		int w, h;
		helper::draw::GetScreenSize(w, h);

		float radius = tan(aimFov)/tan(camFov) * w*0.5f * ((float)3/(float)4);

		helper::draw::SetColor(255, 255, 255, 255);
		interfaces::Surface->DrawOutlinedCircle((int)(w*0.5f), (int)(h*0.5f), (int)(radius), 64);
	}

	void CleanTargetPath()
	{
		/*static float lastcleartime = 0.0f;
		if (state.targetPath.empty())
			return;

		if (interfaces::GlobalVars && (interfaces::GlobalVars->realtime - lastcleartime) < 5.0f)
			return;

		state.targetPath.clear();
		lastcleartime = interfaces::GlobalVars->realtime;
		*/
	}

	void DrawTargetPath()
	{
		/*
		//CleanTargetPath();

		if (state.targetPath.size() < 2)
			return;

		Vector prevScreen;
		if (!helper::engine::WorldToScreen(state.targetPath[0], prevScreen))
			return;

		helper::draw::SetColor({255, 255, 255, 255});

		for (size_t i = 1; i < state.targetPath.size(); i++)
		{
			Vector currScreen;
			if (!helper::engine::WorldToScreen(state.targetPath[i], currScreen))
				continue;

			interfaces::Surface->DrawLine(prevScreen.x, prevScreen.y, currScreen.x, currScreen.y);
			prevScreen = currScreen;
		}*/
	}

	void RunPaint(CTFPlayer* pLocal)
	{
		if (!settings.aimbot.enabled)
			return;

		DrawTargetPath();
		DrawFOVIndicator(pLocal);
	}
};