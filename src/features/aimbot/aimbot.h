#pragma once

#include "../../sdk/definitions/cusercmd.h"
#include "../../sdk/classes/player.h"
#include "../../sdk/classes/weaponbase.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/handle_utils.h"
#include "../entitylist/entitylist.h"

#include "hitscan/aimbot_hitscan.h"
#include "projectile/aimbot_projectile.h"
#include "melee/aimbot_melee.h"
#include <algorithm>

namespace Aimbot
{
	extern AimbotState state;
	Vector GetAngle();
	bool IsRunning();
	bool ShouldSilent();
	void Run(CTFPlayer* pLocal, CTFWeaponBase* pWeapon, CUserCmd* pCmd, bool* bSendPacket);
	void DrawFOVIndicator();
	void CleanTargetPath();
	void DrawTargetPath();
	void RunPaint(CTFPlayer* pLocal);
};