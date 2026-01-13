#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../sdk/definitions/ivmodelrender.h"
#include "../sdk/definitions/keyvalues.h"

#include "../features/chams/chams.h"
#include "../features/glow/glow.h"

DECLARE_VTABLE_HOOK(DrawModelExecute, void, (IVModelRender* thisptr, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld))
{
	//if (settings.esp.chams && !Chams::m_bRunning && Chams::ShouldHide(pInfo.entity_index))
		//return;

	if (interfaces::Engine->IsTakingScreenshot())
		return originalDrawModelExecute(thisptr, state, pInfo, pCustomBoneToWorld);

	if (Chams::ShouldHide(pInfo.entity_index))
		return;

	if (pInfo.entity_index == interfaces::Engine->GetLocalPlayer())
	{
		if (settings.antiaim.enabled)
		{
			// todo
		}
	}

	/*if (settings.esp.chams)
	{
		if (Chams::m_bRunning)
			return originalDrawModelExecute(thisptr, state, pInfo, pCustomBoneToWorld);

		if (Chams::ShouldHide(pInfo.entity_index))
			return;
	}*/

	/*if (settings.esp.blur || settings.esp.stencil)
	{
		if (Glow::m_bRunning)
			return originalDrawModelExecute(thisptr, state, pInfo, pCustomBoneToWorld);
	}*/

	originalDrawModelExecute(thisptr, state, pInfo, pCustomBoneToWorld);
}

inline void HookDrawModelExecute(void)
{
	INSTALL_VTABLE_HOOK(DrawModelExecute, interfaces::ModelRender, 19);
	helper::console::ColoredPrint("IModelRender::DrawModelExecute hooked\n", (Color_t){100, 255, 100, 255});
}