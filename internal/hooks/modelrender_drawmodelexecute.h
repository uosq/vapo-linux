#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../sdk/definitions/ivmodelrender.h"
#include "../sdk/definitions/keyvalues.h"

#include "../features/chams/chams.h"

DECLARE_VTABLE_HOOK(DrawModelExecute, void, (IVModelRender* thisptr, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld))
{
	if (settings.esp.chams && !Chams::m_bRunning && Chams::ShouldHide(pInfo.entity_index))
		return;

	originalDrawModelExecute(thisptr, state, pInfo, pCustomBoneToWorld);
}

inline void HookDrawModelExecute(void)
{
	INSTALL_VTABLE_HOOK(DrawModelExecute, interfaces::ModelRender, 19);
	helper::console::Print("IModelRender::DrawModelExecute hooked\n");
}