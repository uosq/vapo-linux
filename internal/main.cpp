#include "hooks/clientmodeshared_createmove.h"
#include "hooks/ctfplayer_getmaxitemcount.h"
#include "hooks/enginevgui_paint.h"
#include "hooks/framestagenotify.h"
#include "hooks/clientmodeshared_overrideview.h"
#include "hooks/cbaseviewmodel_calcviewmodelview.h"
#include "hooks/cl_checkforpureserverwhitelist.h"

#include "hooks/ipanel_paint_traverse.h"
#include "sdk/interfaces/interfaces.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include "sdk/helpers/fonts.h"
#include "sdk/netvars/netvar.h"

void *MainThread(void*)
{
	if (!InitializeInterfaces())
		return nullptr;

	fontManager.Init();

	SetupNetVars();
	//SetupNetVarsToFile();

	HookCreateMove();
	HookEngineVGuiPaint();
	HookFrameStageNotify();
	HookOverrideView();
	HookCalcViewModelView();
	HookCTFPlayerInventory_MaxItemCount();
	HookPaintTraverse();
	HookCheckForPure();

	return nullptr;
}

__attribute__((constructor))
void init(void)
{
	pthread_t thread;
	pthread_create(&thread, nullptr, MainThread, nullptr);
}