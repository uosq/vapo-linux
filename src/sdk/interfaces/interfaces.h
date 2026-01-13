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

static HCursor cursor = 0;

namespace interfaces
{
	static IVEngineClient014* Engine = nullptr;
	static IBaseClientDLL* BaseClientDLL = nullptr;
	static IClientMode* ClientMode = nullptr;
	static ICvar* Cvar = nullptr;
	static IPanel* VGui = nullptr;
	static ISurface* Surface = nullptr;
	static IClientEntityList* EntityList = nullptr;
	static IEngineVGui* EngineVGui = nullptr;
	static IVRenderView* RenderView = nullptr;
	static IInputSystem* InputSystem = nullptr;
	static IEngineTrace* EngineTrace = nullptr;
	static IMaterialSystem* MaterialSystem = nullptr;
	static CGlobalVars* GlobalVars = nullptr;
	static Prediction* Prediction = nullptr;
	static CInput* CInput = nullptr;
	static IVModelRender* ModelRender = nullptr;
	static IKeyValuesSystem* KeyValuesSystem = nullptr;
	static IVModelInfoClient* ModelInfoClient = nullptr;
	static void* ClientState = nullptr; // fuck C++
	static AttributeManager attributeManager;
}

namespace factories
{
	static CreateInterfaceFn engine = nullptr;
	static CreateInterfaceFn client = nullptr;
	static CreateInterfaceFn vstdlib = nullptr;
	static CreateInterfaceFn vgui = nullptr;
	static CreateInterfaceFn surface = nullptr;
	static CreateInterfaceFn enginevgui = nullptr;
	static CreateInterfaceFn inputsystem = nullptr;
	static CreateInterfaceFn materialsystem = nullptr;
};

template <typename T>
static bool GetInterface(T*& out, CreateInterfaceFn factory, const char* name)
{
	if (out)
		return true;

	out = reinterpret_cast<T*>(factory(name, nullptr));
	return out != nullptr;
}

static bool InitializeInterfaces()
{
	{ // engine factory
		void* enginelib = dlopen("./bin/linux64/engine.so", RTLD_NOLOAD | RTLD_NOW);
		if (!enginelib)
			return false;
	
		factories::engine = reinterpret_cast<CreateInterfaceFn>(dlsym(enginelib, "CreateInterface"));
	}

	{ // baseClientDll factory
		void* clientlib = dlopen("./tf/bin/linux64/client.so", RTLD_NOLOAD | RTLD_NOW);
		if (!clientlib)
			return false;
	
		factories::client = reinterpret_cast<CreateInterfaceFn>(dlsym(clientlib, "CreateInterface"));
	}

	{ // vstdlib factory (console commands)
		void* vstdlib = dlopen("./bin/linux64/libvstdlib.so", RTLD_NOLOAD | RTLD_NOW);
		if (!vstdlib)
			return false;
	
		factories::vstdlib = reinterpret_cast<CreateInterfaceFn>(dlsym(vstdlib, "CreateInterface"));

		interfaces::KeyValuesSystem = reinterpret_cast<IKeyValuesSystem*>(dlsym(vstdlib, "KeyValuesSystem"));
		if (!interfaces::KeyValuesSystem)
			return false;
	}

	{ // vgui2 factory (paint traverse)
		void* vgui2 = dlopen("./bin/linux64/vgui2.so", RTLD_NOLOAD | RTLD_NOW);
		if (!vgui2)
			return false;

		factories::vgui = reinterpret_cast<CreateInterfaceFn>(dlsym(vgui2, "CreateInterface"));
	}

	{ // surface factory (draw library)
		void* surface = dlopen("./bin/linux64/vguimatsurface.so", RTLD_NOLOAD | RTLD_NOW);
		if (!surface)
			return false;

		factories::surface = reinterpret_cast<CreateInterfaceFn>(dlsym(surface, "CreateInterface"));
	}

	{ // inputsystem factory
		void* inputsystem = dlopen("./bin/linux64/inputsystem.so", RTLD_NOLOAD | RTLD_NOW);
		if (!inputsystem)
			return false;

		factories::inputsystem = reinterpret_cast<CreateInterfaceFn>((dlsym(inputsystem, "CreateInterface")));
	}

	{	// material system factory
		void *materialsystem = dlopen("./bin/linux64/materialsystem.so", RTLD_NOLOAD | RTLD_NOW);
		if (!materialsystem)
			return false;

		factories::materialsystem = reinterpret_cast<CreateInterfaceFn>(dlsym(materialsystem, "CreateInterface"));
	}

	// get interfaces
	// i should probably check if they return false
	
	if (!GetInterface(interfaces::Engine, factories::engine, "VEngineClient014"))
		return false;
	
	if (!GetInterface(interfaces::EngineVGui, factories::engine, "VEngineVGui002"))
		return false;
	
	if (!GetInterface(interfaces::BaseClientDLL, factories::client, "VClient017"))
		return false;
	
	if (!GetInterface(interfaces::Cvar, factories::vstdlib, "VEngineCvar004"))
		return false;
	
	if (!GetInterface(interfaces::VGui, factories::vgui, "VGUI_Panel009"))
		return false;
	
	if (!GetInterface(interfaces::Surface, factories::surface, "VGUI_Surface030"))
		return false;
	
	if (!GetInterface(interfaces::EntityList, factories::client, "VClientEntityList003"))
		return false;
	
	if (!GetInterface(interfaces::RenderView, factories::engine, "VEngineRenderView014"))
		return false;
	
	if (!GetInterface(interfaces::InputSystem, factories::inputsystem, "InputSystemVersion001"))
		return false;
	
	if (!GetInterface(interfaces::EngineTrace, factories::engine, "EngineTraceClient003"))
		return false;
	
	if (!GetInterface(interfaces::MaterialSystem, factories::materialsystem, "VMaterialSystem082"))
		return false;

	if (!GetInterface(interfaces::Prediction, factories::client, "VClientPrediction001"))
		return false;

	/*if (!GetInterface(interfaces::StudioRender, factories::client, "VStudioRender025"))
		return false;*/

	if (!GetInterface(interfaces::ModelRender, factories::engine, "VEngineModel016"))
		return false;

	//if (!GetInterface(interfaces::GameMovement, factories::client, "GameMovement001"))
		//return false;

	if (!GetInterface(interfaces::ModelInfoClient, factories::engine, "VModelInfoClient006"))
		return false;

	{ // ClientModeShared
		uintptr_t leaInstr = (uintptr_t)sigscan_module("client.so", "48 8D 05 ? ? ? ? 40 0F B6 F6 48 8B 38");
		uintptr_t g_pClientMode_addr = vtable::ResolveRIP(leaInstr, 3, 7); // lea rax, [g_pClientMode]
		interfaces::ClientMode = *reinterpret_cast<IClientMode**>(g_pClientMode_addr);;
		if (!interfaces::ClientMode)
			return false;
	}

	{ // global vars
		uintptr_t HudUpdateFn = (uintptr_t)vtable::get(interfaces::BaseClientDLL)[11];
		unsigned int mov_addr = *(unsigned int*)(HudUpdateFn + 0x16);
		uintptr_t next_instr = (uintptr_t)(HudUpdateFn + 0x1A);
		
		// MOV RAX,qword ptr [gpGlobals]
		interfaces::GlobalVars = (CGlobalVars*)(*(void **)(next_instr + mov_addr));
		if (!interfaces::GlobalVars)
			return false;
	}

	{ // attribute hook manager
		// get the function
		attribute_hook_value_float_original = (float (*)(float, const char*, CBaseEntity*, void*, bool))sigscan_module("client.so", "55 31 C0 48 89 E5 41 57 41 56 41 55 49 89 F5 41 54 49 89 FC 53 89 CB");
		if (!attribute_hook_value_float_original)
			return false;
	}

	{ // CInput
		uintptr_t leaInstr = (uintptr_t)(sigscan_module("client.so", "48 8D 05 ? ? ? ? 48 8B 38 48 8B 07 FF 90 ? ? ? ? 48 8D 15 ? ? ? ? 84 C0"));
		uintptr_t g_Input_addr = vtable::ResolveRIP(leaInstr, 3, 7);
		interfaces::CInput = *reinterpret_cast<CInput**>(g_Input_addr);
		if (!interfaces::CInput)
			return false;
	}

	{ // ClientState
		// xref: Setting rcon_address %s:%d\n
		// then scroll down a bit until you find
		// 4C 8B 40 20     MOV        R8,qword ptr [RAX + 0x20]=>clientstate_cl
		uintptr_t movInstr = reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "48 8D 05 ? ? ? ? 4C 8B 40"));
		uintptr_t g_ClientState = vtable::ResolveRIP(movInstr, 3, 7);
		interfaces::ClientState = reinterpret_cast<void*>(g_ClientState); // fuck c++
	}

	return true;
}