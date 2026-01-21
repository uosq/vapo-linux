#include "hooks/cinventorymanager_showitemspickedup.h"
#include "hooks/clientmodeshared_createmove.h"
#include "hooks/ctfplayer_getmaxitemcount.h"
#include "hooks/host_issecureserverallowed.h"
#include "hooks/isurface_setcursor.h"
#include "hooks/modelrender_drawmodelexecute.h"
#include "hooks/enginevgui_paint.h"
#include "hooks/framestagenotify.h"
#include "hooks/clientmodeshared_overrideview.h"
#include "hooks/cbaseviewmodel_calcviewmodelview.h"
#include "hooks/cl_checkforpureserverwhitelist.h"
#include "hooks/clientmodeshared_dopostscreenspaceeffects.h"
#include "hooks/chlclient_levelshutdown.h"
#include "hooks/ipanel_paint_traverse.h"
#include "hooks/netchan_sendnetmsg.h"
#include "hooks/sdl.h"
#include "hooks/vulkan.h"
#include "hooks/chlclient_levelinitpreentity.h"
#include "hooks/chlclient_levelpostentity.h"

#include "sdk/interfaces/interfaces.h"
#include <sys/types.h>
#include <unistd.h>
#include "sdk/helpers/fonts.h"
#include "sdk/netvars/netvar.h"
#include "features/lua/api.h"

__attribute__((constructor))
void init(void)
{
	if (!InitializeInterfaces())
		return;

	fontManager.Init();

	HookSDL();
	HookVulkan();

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
	HookDrawModelExecute();
	HookDoPostScreenSpaceEffects();
	HookLockCursor();
	HookLevelShutdown();
	HookShowItemsPickedUp();
	HookSendNetMsg();
	HookLevelInitPostEntity();
	HookLevelInitPreEntity();
	HookIsSecureServerAllowed();

	Lua::InitLua();
}

__attribute__((destructor))
void uninit(void)
{
	Lua::CloseLua();
}