#pragma once

#include "../libdetour/libdetour.h"
#include "../libsigscan.h"
#include "../settings.h"
#include "../vtables.h"

using Host_IsSecureServerAllowedFn = bool(*)(void);
inline detour_ctx_t issecure_ctx;
inline Host_IsSecureServerAllowedFn original_IsSecureServerAllowed = nullptr;

inline bool Hooked_HostIsSecureServerAllowed(void)
{
	if (settings.misc.insecure_mode_bypass)
	{
		// sig inside Host_IsSecureServerAllowed
		static uintptr_t g_pAllowSecureServers_leaInstr = reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "48 8D 05 ? ? ? ? 31 C9 BA 01 00 00 00"));
		static int* g_pAllowSecureServers = reinterpret_cast<int*>(vtable::ResolveRIP(g_pAllowSecureServers_leaInstr, 3, 7));
		*g_pAllowSecureServers = 1;
		return true;
	}

	return original_IsSecureServerAllowed();
}

inline void HookIsSecureServerAllowed(void)
{
	// xref: -textmode

	original_IsSecureServerAllowed = reinterpret_cast<Host_IsSecureServerAllowedFn>(sigscan_module("engine.so", "55 48 89 E5 E8 ? ? ? ? 48 8D 35 ? ? ? ? 48 89 C7 48 8B 00 FF 50 50 85 C0 74 ? 31 C0"));
	if (original_IsSecureServerAllowed == nullptr)
		return;

	detour_init(&issecure_ctx, (void*)original_IsSecureServerAllowed, (void*)&Hooked_HostIsSecureServerAllowed);
	detour_enable(&issecure_ctx);
}