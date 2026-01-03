#pragma once

// sig 83 3d fd 8a 65 00 01 7e 77 80 3d a8 13 66 00 00 75 6e 48 8d 05 af 96 6b

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
	originalCheckForPureServerWhitelistFn = (CL_CheckForPureServerWhitelistFn)sigscan_module("engine.so", "83 3D FD 8A 65 00 01 7E 77 80 3D A8 13 66 00 00 75 6E 48 8D 05 AF 96 6B");
	detour_init(&CL_CheckForPureServerWhitelist_ctx, (void*)originalCheckForPureServerWhitelistFn, (void*)&Hooked_CL_CheckForPureServerWhitelist);
	detour_enable(&CL_CheckForPureServerWhitelist_ctx);

	Color_t color{100, 255, 100, 255};
	interfaces::Cvar->ConsoleColorPrintf(color, "CL_CheckForPureServerWhitelist hooked\n");
}