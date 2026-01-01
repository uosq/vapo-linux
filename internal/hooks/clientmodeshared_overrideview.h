#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"
#include <string>

using OverrideViewFn = void (*)(IClientMode *thisptr, CViewSetup *view);
inline OverrideViewFn originalOverrideView = nullptr;

inline void HookedOverrideView(IClientMode *thisptr, CViewSetup *view)
{
	float originalFov = view->fov;

	originalOverrideView(thisptr, view);

	if (settings.misc.customfov)
	{
		view->fov = settings.misc.customfov;
	}
}

inline void HookOverrideView()
{
	void** vt = vtable::get(interfaces::clientMode);
	originalOverrideView = vtable::hook(vt, 17, &HookedOverrideView);

	helper::console::ColoredPrint("ClientModeShared::OverrideView hooked\n", (Color_t){100, 255, 100, 255});
}