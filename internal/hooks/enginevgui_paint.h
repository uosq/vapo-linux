#pragma once

#include "../sdk/definitions/ipanel.h"
#include <cwchar>
#include <string>
#include "../sdk/classes/entity.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"
#include "../sdk/definitions/color.h"
#include "../sdk/classes/weaponbase.h"
#include "../sdk/handle_utils.h"

#include "../features/esp/esp.h"
//#include "../httplib.h"

#include "../features/aimbot/aimbot.h"
#include "../features/visuals/visuals.h"

using VGUIPaintFn = void (*)(IEngineVGui* thisptr, VGuiPanel_t type);
inline VGUIPaintFn originalVGuiPaint = nullptr;

inline void HookedEngineVGuiPaint(IEngineVGui* thisptr, VGuiPanel_t type)
{
	originalVGuiPaint(thisptr, type);

	if (type & PAINT_INGAMEPANELS)
	{
		HFont font = helper::draw::GetCurrentFont();
		helper::draw::SetFont(font);
	
		Color color{255, 255, 255, 255};
		helper::draw::TextShadow(10, 10, color, "Vapo Linux");
	
		CTFPlayer* pLocal = helper::engine::GetLocalPlayer();
		if (!pLocal)
			return;

		Visuals::spectatorlist.Run(pLocal);
		ESP::Run(pLocal);
	
		CTFWeaponBase* pWeapon = HandleAs<CTFWeaponBase>(pLocal->GetActiveWeapon());
		if (!pWeapon)
			return;
	
		Aimbot::DrawTargetPath();
		Aimbot::DrawFOVIndicator(pLocal, pWeapon);
	}
}

inline void HookEngineVGuiPaint()
{
	auto vt = vtable::get(interfaces::EngineVGui);
	originalVGuiPaint = vtable::hook(vt, 15, HookedEngineVGuiPaint);
	
	/*
		reminder to me:
		if using windows hooks, always add a +1, as (i think)
		gcc/clang puts the constructor&destructor at the start of the class
	*/

	Color_t color;
	color.SetRGB(100, 255, 100, 255);
	helper::console::ColoredPrint("EngineVGui::Paint hooked\n", color);
}