#pragma once

#include <dlfcn.h>

#include "../definitions/ivengineclient.h"
#include "../definitions/iinput.h"
#include "../definitions/iclientmode.h"
#include "../definitions/cvar.h"
#include "../../libsigscan.h"
#include "../../vtables.h"
#include "createinterface.h"
#include "../definitions/isurface.h"
#include "../definitions/icliententitylist.h"
#include "../definitions/ienginevgui.h"
#include "../definitions/enginetool.h"
#include "../definitions/cglobalvars.h"
#include "../definitions/ivrenderview.h"
#include "../definitions/attributemanager.h"
#include "../definitions/weaponinfo.h"
#include "../definitions/iinputsystem.h"
#include "../definitions/ienginetrace.h"
#include "../definitions/cvar.h"
#include "../definitions/imaterialsystem.h"
#include "../definitions/iprediction.h"
#include "../definitions/ivmodelrender.h"
#include "../definitions/ikeyvaluessystem.h"
#include "../definitions/igamemovement.h"
#include "../definitions/imovehelper.h"
#include "../definitions/ivmodelinfoclient.h"

extern HCursor cursor;

namespace interfaces
{
	extern IVEngineClient014* Engine;
	extern IBaseClientDLL* BaseClientDLL;
	extern IClientMode* ClientMode;
	extern ICvar* Cvar;
	extern IPanel* VGui;
	extern ISurface* Surface;
	extern IClientEntityList* EntityList;
	extern IEngineVGui* EngineVGui;
	extern IVRenderView* RenderView;
	extern IInputSystem* InputSystem;
	extern IEngineTrace* EngineTrace;
	extern IMaterialSystem* MaterialSystem;
	extern CGlobalVars* GlobalVars;
	extern Prediction* Prediction;
	extern CInput* CInput;
	extern IVModelRender* ModelRender;
	extern IKeyValuesSystem* KeyValuesSystem;
	extern IVModelInfoClient* ModelInfoClient;
	extern void* ClientState;
	extern AttributeManager attributeManager;
}

namespace factories
{
	extern CreateInterfaceFn engine;
	extern CreateInterfaceFn client;
	extern CreateInterfaceFn vstdlib;
	extern CreateInterfaceFn vgui;
	extern CreateInterfaceFn surface;
	extern CreateInterfaceFn enginevgui;
	extern CreateInterfaceFn inputsystem;
	extern CreateInterfaceFn materialsystem;
};

template <typename T>
bool GetInterface(T*& out, CreateInterfaceFn factory, const char* name);
bool InitializeInterfaces();