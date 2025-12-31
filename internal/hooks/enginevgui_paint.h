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
#include "../httplib.h"

//using PaintTraverseFn = void (*)(IPanel* thisptr, VPANEL vguiPanel, bool forceRepaint, bool allowForce);
using PaintTraverseFn = void (*)(IEngineVGui* thisptr, VGuiPanel_t type);
inline PaintTraverseFn originalPaintTraverse = nullptr;

static inline httplib::Client cli("http://127.0.0.1:6969");

inline void HookedEngineVGuiPaint(IEngineVGui* thisptr, VGuiPanel_t type)
{
	originalPaintTraverse(thisptr, type);

	//interfaces::surface->DrawSetColor(40, 40, 40, 255);
	//helper::draw::FilledRect(100, 100, 800, 800);

	HFont font = helper::draw::GetCurrentFont();
	helper::draw::SetFont(font);
	//helper::draw::SetColor(255, 255, 255, 255);
	//helper::draw::FilledRect(0, 0, 100, 100);

	CTFPlayer* pLocal = helper::engine::GetLocalPlayer();
	if (!pLocal)
		return;

	CTFWeaponBase* pWeapon = HandleAs<CTFWeaponBase>(pLocal->GetActiveWeapon());
	if (!pWeapon)
		return;

	if (auto response = cli.Get("/"))
	{
		nlohmann::json j = nlohmann::json::parse(response->body);
		settings = j.get<Settings>();
	}
	//interfaces::vstdlib->ConsolePrintf("%d\n", weapon->m_iClip1());

	ESP::Run(pLocal, pWeapon);

	/*int width, height;
	std::wstring text = helper::draw::ConvertStringToWChar("Hello, world!");
	interfaces::surface->GetTextSize(font, text.c_str(), width, height);

	helper::draw::Text(50, 50, (Color){255, 255, 255, 255}, "Hello, world!\n");*/
}

inline void HookEngineVGuiPaint()
{
	auto vt = vtable::get(interfaces::enginevgui);
	originalPaintTraverse = vtable::hook(vt, 15, HookedEngineVGuiPaint);
	
	/*
		reminder to me:
		if using windows hooks, always add a +1, as (i think)
		gcc/clang puts the constructor&destructor at the start of the class
	*/

	Color_t color;
	color.SetRGB(100, 255, 100, 255);
	helper::console::ColoredPrint("EngineVGui::Paint hooked\n", color);
}