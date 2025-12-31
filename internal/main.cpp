#include "hooks/clientmodeshared_createmove.h"
#include "hooks/enginevgui_paint.h"
#include "sdk/interfaces/interfaces.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include "sdk/helpers/fonts.h"
#include "sdk/netvars/netvar.h"

#define PrintMsg(text) write(2, text, sizeof(text) - 1)

void *MainThread(void*)
{
	if (!InitializeInterfaces())
		return nullptr;

	fontManager.Init();

	SetupNetVars();
	//SetupNetVarsToFile();

	HookCreateMove();
	HookEngineVGuiPaint();

	return nullptr;
}

__attribute__((constructor))
void init(void)
{
    pthread_t thread;
    pthread_create(&thread, NULL, MainThread, (void*)0);
}