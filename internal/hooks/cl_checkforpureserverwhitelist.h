#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../settings.h"
#include "../libdetour/libdetour.h"

using CL_CheckForPureServerWhitelistFn = void(*)(void*& pFilesToReload);
inline CL_CheckForPureServerWhitelistFn originalCheckForPureServerWhitelistFn = nullptr;

DETOUR_DECL_TYPE(void, originalCheckForPureServerWhitelistFn, void*& pFilesToReload);

static detour_ctx_t CL_CheckForPureServerWhitelist_ctx;

inline void Hooked_CL_CheckForPureServerWhitelist(void*& pFilesToReload)
{
	if (settings.misc.sv_pure_bypass)
		return;

	DETOUR_ORIG_CALL(&CL_CheckForPureServerWhitelist_ctx, originalCheckForPureServerWhitelistFn, pFilesToReload);
}

inline void HookCheckForPure()
{
	originalCheckForPureServerWhitelistFn = (CL_CheckForPureServerWhitelistFn)sigscan_module("engine.so", "83 3D ? ? ? ? 01 7E ? 80 3D ? ? ? ? 00 75");
	detour_init(&CL_CheckForPureServerWhitelist_ctx, (void*)originalCheckForPureServerWhitelistFn, (void*)&Hooked_CL_CheckForPureServerWhitelist);
	detour_enable(&CL_CheckForPureServerWhitelist_ctx);

	Color_t color{100, 255, 100, 255};
	interfaces::Cvar->ConsoleColorPrintf(color, "CL_CheckForPureServerWhitelist hooked\n");
}