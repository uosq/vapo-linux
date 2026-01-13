#pragma once

#include "../../sdk/interfaces/interfaces.h"
#include "../../sdk/definitions/cusercmd.h"
#include "../../settings.h"
#include "../../sdk/classes/player.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/definitions/cclientstate.h"

namespace Antiaim
{
	static std::string GetPitchModeName(PitchMode mode)
	{
		switch (settings.antiaim.pitch_mode)
		{
			case PitchMode::NONE: return "None";
			case PitchMode::UP: return "Up";
			case PitchMode::DOWN: return "Down";
			case PitchMode::FAKEUP: return "Fake Up";
			case PitchMode::FAKEDOWN: return "Fake Down";
			case PitchMode::RANDOM: return "Random";
		}

		return "unknown";
	}

	static std::string GetYawModeName(YawMode mode)
	{
		switch (mode)
		{
			case YawMode::LEFT: return "Left";
			case YawMode::RIGHT: return "Right";
			case YawMode::BACK: return "Back";
			case YawMode::NONE: return "None";
			case YawMode::FORWARD: return "Forward";
			case YawMode::SPIN_LEFT: return "Spin left";
			case YawMode::SPIN_RIGHT: return "Spin Right";
			case YawMode::JITTER: return "Jitter";
		}

		return "unknown";
	}

	static float GetPitch(CUserCmd* pCmd)
	{
		switch (settings.antiaim.pitch_mode)
		{
			case PitchMode::NONE: return pCmd->viewangles.x;
			case PitchMode::UP: return -89.0f;
			case PitchMode::DOWN: return 89.0f;
			case PitchMode::FAKEUP: return -271.0f;
			case PitchMode::FAKEDOWN: return 271.0f;
			case PitchMode::RANDOM: return (rand() % 180) * (rand() % 2 ? 1 : -1);
		}

		return 0;
	}

	static float GetYaw(CUserCmd* pCmd, YawMode mode)
	{
		switch (mode)
		{
			case YawMode::LEFT: return pCmd->viewangles.y + 90.0f;
			case YawMode::RIGHT: return pCmd->viewangles.y - 90.0f;
			case YawMode::BACK: return pCmd->viewangles.y - 180.0f;
			case YawMode::NONE: return pCmd->viewangles.y;
			case YawMode::FORWARD: return pCmd->viewangles.y;
			case YawMode::SPIN_LEFT: return pCmd->viewangles.y + (pCmd->tick_count % 360);
			case YawMode::SPIN_RIGHT: return pCmd->viewangles.y - (pCmd->tick_count % 360);
			case YawMode::JITTER: return pCmd->viewangles.y + ((pCmd->tick_count % 2) ? 90.0f : -90.0f);
		}

		return 0;
	}

	static void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, bool* bSendPacket)
	{
		if (!settings.antiaim.enabled)
			return;

		if (!pLocal->IsAlive() || pLocal->IsGhost() || pLocal->IsTaunting())
			return;

		if (helper::localplayer::CanShoot(pLocal, pWeapon) && (pCmd->buttons & IN_ATTACK))
			return;

		constexpr int maxChoke = 2; // fake on 2 ticks, real on 1
		int choke = reinterpret_cast<CClientState*>(interfaces::ClientState)->chokedcommands;

		if (choke < maxChoke) *bSendPacket = false; // fake
		else *bSendPacket = true; // real

		bool isFake = !(*bSendPacket);
		
		if (settings.antiaim.pitch_mode != PitchMode::NONE)
		{
			float pitch = GetPitch(pCmd);
			pCmd->viewangles.x = pitch;
		}

		YawMode mode = isFake ? settings.antiaim.fake_yaw_mode : settings.antiaim.real_yaw_mode;

		if (mode != YawMode::NONE)
			pCmd->viewangles.y = GetYaw(pCmd, mode);
	}
}