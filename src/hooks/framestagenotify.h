#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/helpers/helper.h"
#include "../features/visuals/visuals.h"
#include "../features/entitylist/entitylist.h"
#include "../features/visuals/customfov/customfov.h"

#include "../features/lua/hooks.h"
#include "../features/lua/api.h"

DECLARE_VTABLE_HOOK(FrameStageNotify, void, (CHLClient* thisptr, int stage))
{
	switch (stage)
	{
		case FRAME_RENDER_START:
		{
			if (settings.misc.thirdperson)
			{
				CTFPlayer* pLocal = EntityList::GetLocal();
				if (pLocal == nullptr || !pLocal->IsAlive())
					break;

				interfaces::Prediction->SetLocalViewAngles(helper::localplayer::LastAngle);
			}
			break;
		}
		case FRAME_NET_UPDATE_END:
		{
			EntityList::Store();

			CTFPlayer* pLocal = EntityList::GetLocal();
			if (pLocal == nullptr)
				break;

			Visuals::thirdperson.Run(pLocal);
			break;
		}

		default: break;
	}

	if (LuaHookManager::HasHooks("FrameStageNotify"))
	{
		lua_pushinteger(Lua::m_luaState, stage);
		LuaHookManager::Call(Lua::m_luaState, "FrameStageNotify", 1);
	}

	originalFrameStageNotify(thisptr, stage);
}

inline void HookFrameStageNotify()
{
	INSTALL_VTABLE_HOOK(FrameStageNotify, interfaces::ClientDLL, 35);

	constexpr Color_t color = {100, 255, 100, 255};
	helper::console::ColoredPrint("IBaseClientDll::FrameStageNotify hooked\n", color);
}