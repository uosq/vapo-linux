#pragma once

#include "../sdk/definitions/ipanel.h"
#include "../sdk/interfaces/interfaces.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"

using PaintTraverseFn = void (*)(IPanel* thisptr, VPANEL vguiPanel, bool forceRepaint, bool allowForce);
inline PaintTraverseFn originalPaintTraverse = nullptr;

static inline float lastSettingsUpdate = 0.0f;

inline void HookedPaintTraverse(IPanel* thisptr, VPANEL vguiPanel, bool forceRepaint, bool allowForce)
{
	float currenttime = interfaces::GlobalVars ? interfaces::GlobalVars->realtime : 0.0f;

	// is this a good way of doing it? absolutely fucking not
	// but im lazy and dont want to hook sdl and vulkan
	if (currenttime - lastSettingsUpdate > 1.0f)
	{
		if (auto response = cli.Get("/"))
		{
			nlohmann::json j = nlohmann::json::parse(response->body);
			settings = j.get<Settings>();
			//helper::console::Print("Updated settings!\n");
		}
		lastSettingsUpdate = currenttime;
	}

	const char* panelName = interfaces::VGui->GetName(vguiPanel);

	// https://github.com/rei-2/Amalgam/blob/master/Amalgam/src/Hooks/IPanel_PaintTraverse.cpp
	if (settings.misc.streamer_mode)
	{
		switch (fnv::Hash(panelName))
		{
			case fnv::HashConst("SteamFriendsList"):
			case fnv::HashConst("avatar"):
			case fnv::HashConst("RankPanel"):
			case fnv::HashConst("ModelContainer"):
			case fnv::HashConst("ServerLabelNew"):
			return;
		}
	}

	originalPaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);
}

inline void HookPaintTraverse()
{
	//GUI_InitWindow(windowContext, 10, 10, 200, 200);

	void** vt = vtable::get(interfaces::VGui);
	originalPaintTraverse = vtable::hook(vt, 42, &HookedPaintTraverse);

	helper::console::ColoredPrint("IPanel::PaintTraverse hooked\n", (Color_t){100, 255, 100, 255});
}