#pragma once

#include "../sdk/definitions/ipanel.h"
#include <cwchar>
#include <string>
#include "../sdk/classes/entity.h"
#include "../sdk/helpers/helper.h"
#include "../sdk/definitions/color.h"
#include "../sdk/classes/weaponbase.h"
#include "../sdk/handle_utils.h"
#include "../settings.h"

#include "../features/esp/esp.h"
#include "../features/aimbot/aimbot.h"
#include "../features/visuals/visuals.h"

DECLARE_VTABLE_HOOK(VGuiPaint, void, (IEngineVGui* thisptr, VGuiPanel_t type))
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

		if (interfaces::CInput != nullptr)
			if (interfaces::CInput->CAM_IsThirdPerson())
				helper::draw::TextShadow(10, 30, color, "Is Thirdperson");

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
	INSTALL_VTABLE_HOOK(VGuiPaint, interfaces::EngineVGui, 15);

	Color_t color;
	color.SetRGB(100, 255, 100, 255);
	helper::console::ColoredPrint("EngineVGui::Paint hooked\n", color);
}