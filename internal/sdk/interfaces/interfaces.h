#pragma once

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
#include <dlfcn.h>

//static GetTFWeaponInfoFn GetTFWeaponInfo;

namespace interfaces
{
	inline IVEngineClient014* engine = nullptr;
	inline IBaseClientDLL* baseClientDll = nullptr;
	inline IClientMode* clientMode = nullptr;
	inline ICvar* vstdlib = nullptr;
	inline void* vgui = nullptr;
	inline ISurface* surface = nullptr;
	inline IClientEntityList* entitylist = nullptr;
	inline IEngineVGui* enginevgui = nullptr;
	inline IEngineTool* enginetool = nullptr;
	inline IVRenderView* renderview = nullptr;
	inline IInputSystem* inputsystem = nullptr;
	inline AttributeManager attributeManager;
}

namespace factories
{
	inline CreateInterfaceFn engine = nullptr;
	inline CreateInterfaceFn client = nullptr;
	inline CreateInterfaceFn vstdlib = nullptr;
	inline CreateInterfaceFn vgui = nullptr;
	inline CreateInterfaceFn surface = nullptr;
	inline CreateInterfaceFn enginevgui = nullptr;
	inline CreateInterfaceFn inputsystem = nullptr;
};

template <typename T>
inline bool GetInterface(T*& out, CreateInterfaceFn factory, const char* name)
{
	if (out)
		return true;

	out = reinterpret_cast<T*>(factory(name, nullptr));
	return out != nullptr;
}

inline bool InitializeInterfaces()
{
	{ // engine factory
		void* enginelib = dlopen("./bin/linux64/engine.so", RTLD_NOLOAD | RTLD_NOW);
		if (!enginelib)
			return false;
	
		factories::engine = (CreateInterfaceFn)dlsym(enginelib, "CreateInterface");
	}

	{ // baseClientDll factory
		void* clientlib = dlopen("./tf/bin/linux64/client.so", RTLD_NOLOAD | RTLD_NOW);
		if (!clientlib)
			return false;
	
		factories::client = (CreateInterfaceFn)dlsym(clientlib, "CreateInterface");
	}

	{ // vstdlib factory (console commands)
		void* vstdlib = dlopen("./bin/linux64/libvstdlib.so", RTLD_NOLOAD | RTLD_NOW);
		if (!vstdlib)
			return false;
	
		factories::vstdlib = (CreateInterfaceFn)dlsym(vstdlib, "CreateInterface");
	}

	{ // vgui2 factory (paint traverse)
		void* vgui2 = dlopen("./bin/linux64/vgui2.so", RTLD_NOLOAD | RTLD_NOW);
		if (!vgui2)
			return false;

		factories::vgui = (CreateInterfaceFn)dlsym(vgui2, "CreateInterface");
	}

	{ // surface factory (draw library)
		void* surface = dlopen("./bin/linux64/vguimatsurface.so", RTLD_NOLOAD | RTLD_NOW);
		if (!surface)
			return false;

		factories::surface = (CreateInterfaceFn)dlsym(surface, "CreateInterface");
	}

	{ // inputsystem factory
		void* inputsystem = dlopen("./bin/linux64/inputsystem.so", RTLD_NOLOAD | RTLD_NOW);
		if (!inputsystem)
			return false;

		factories::inputsystem = (CreateInterfaceFn)(dlsym(inputsystem, "CreateInterface"));
	}

	// get interfaces
	// i should probably check if they return false
	GetInterface(interfaces::engine, factories::engine, "VEngineClient014");
	GetInterface(interfaces::enginevgui, factories::engine, "VEngineVGui002");
	GetInterface(interfaces::baseClientDll, factories::client, "VClient017");
	GetInterface(interfaces::vstdlib, factories::vstdlib, "VEngineCvar004");
	GetInterface(interfaces::vgui, factories::vgui, "VGUI_Panel009");
	GetInterface(interfaces::surface, factories::surface, "VGUI_Surface030");
	GetInterface(interfaces::entitylist, factories::client, "VClientEntityList003");
	GetInterface(interfaces::enginetool, factories::engine, "VENGINETOOL003");
	GetInterface(interfaces::renderview, factories::engine, "VEngineRenderView014");
	GetInterface(interfaces::inputsystem, factories::inputsystem, "InputSystemVersion001");

	{ // ClientModeShared
		uintptr_t leaInstr = (uintptr_t)sigscan_module("client.so", "48 8D 05 ? ? ? ? 40 0F B6 F6 48 8B 38");
		uintptr_t g_pClientMode_addr = vtable::ResolveRIP(leaInstr, 3, 7); // lea rax, [g_pClientMode]
		interfaces::clientMode = *reinterpret_cast<IClientMode**>(g_pClientMode_addr);;
	}

	{ // global vars
		uintptr_t HudUpdateFn = (uintptr_t)vtable::get(interfaces::baseClientDll)[11];
		unsigned int mov_addr = *(unsigned int*)(HudUpdateFn + 0x16);
		uintptr_t next_instr = (uintptr_t)(HudUpdateFn + 0x1A);
		
		// MOV RAX,qword ptr [gpGlobals]
		globalvars = (CGlobalVars*)(*(void **)(next_instr + mov_addr));
		//interfaces::vstdlib->ConsolePrintf("%f\n", globalvars->interval_per_tick);
	}

	{ // attribute hook manager
		// get the function
		attribute_hook_value_float_original = (float (*)(float, const char*, CBaseEntity*, void*, bool))sigscan_module("client.so", "55 31 C0 48 89 E5 41 57 41 56 41 55 49 89 F5 41 54 49 89 FC 53 89 CB");
	}

	{ // hook GetTFWeaponInfo(int weaponID)
		// 83 FF 6D 77 13 48 8D 05 ? ? ? ? 48 63 FF 48 8B 04 F8 C3 works but gives garbage data
		//GetTFWeaponInfo = reinterpret_cast<GetTFWeaponInfoFn>(sigscan_module("client.so", "0F B7 BF 12 0F 00 00 E9 54 15 21"));
	}

	return true;
}