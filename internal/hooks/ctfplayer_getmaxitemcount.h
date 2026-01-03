#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"
#include "../libdetour/libdetour.h"

#include <string>

using GetMaxItemCountFn = int(*)(void* thisptr);
inline GetMaxItemCountFn originalMaxItemCountFn = nullptr;

DETOUR_DECL_TYPE(int, originalMaxItemCountFn, void* thisptr);

static detour_ctx_t GetMaxItemCount_ctx;

inline int Hooked_GetMaxItemCount(void* thisptr)
{
	if (settings.misc.backpack_expander)
		return 4000;

	int ret;
	DETOUR_ORIG_GET(&GetMaxItemCount_ctx, ret, originalMaxItemCountFn, thisptr);

	return ret;
}

inline void HookCTFPlayerInventory_MaxItemCount()
{
	originalMaxItemCountFn = (GetMaxItemCountFn)sigscan_module("client.so", "48 8B BF 98 00 00 00 48 85 FF 74 54 55 BE 07 00 00 00 48 89 E5 E8 C6 4E 6F 00 48 85 C0 74 39 83 78 28 01");
	detour_init(&GetMaxItemCount_ctx, (void*)originalMaxItemCountFn, (void*)&Hooked_GetMaxItemCount);
	detour_enable(&GetMaxItemCount_ctx);

	Color_t color{100, 255, 100, 255};
	interfaces::Cvar->ConsoleColorPrintf(color, "CTFPlayerInventory::GetMaxItemCount hooked\n");
}