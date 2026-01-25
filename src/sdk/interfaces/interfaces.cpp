#include "interfaces.h"

HCursor cursor = 0;
AttributeHookValueFn AttributeHookValue = nullptr;

namespace interfaces
{
	IVEngineClient014* Engine = nullptr;
	IBaseClientDLL* BaseClientDLL = nullptr;
	IClientMode* ClientMode = nullptr;
	ICvar* Cvar = nullptr;
	IPanel* VGui = nullptr;
	ISurface* Surface = nullptr;
	IClientEntityList* EntityList = nullptr;
	IEngineVGui* EngineVGui = nullptr;
	IVRenderView* RenderView = nullptr;
	IInputSystem* InputSystem = nullptr;
	IEngineTrace* EngineTrace = nullptr;
	IMaterialSystem* MaterialSystem = nullptr;
	CGlobalVars* GlobalVars = nullptr;
	class Prediction* Prediction = nullptr;
	class CInput* CInput = nullptr;
	IVModelRender* ModelRender = nullptr;
	IKeyValuesSystem* KeyValuesSystem = nullptr;
	IVModelInfoClient* ModelInfoClient = nullptr;
	void* ClientState = nullptr; // fuck C++
}

namespace factories
{
	CreateInterfaceFn engine = nullptr;
	CreateInterfaceFn client = nullptr;
	CreateInterfaceFn vstdlib = nullptr;
	CreateInterfaceFn vgui = nullptr;
	CreateInterfaceFn surface = nullptr;
	CreateInterfaceFn enginevgui = nullptr;
	CreateInterfaceFn inputsystem = nullptr;
	CreateInterfaceFn materialsystem = nullptr;
};

template <typename T>
bool GetInterface(T*& out, CreateInterfaceFn factory, const char* name)
{
	if (out)
		return true;

	out = reinterpret_cast<T*>(factory(name, nullptr));
	return out != nullptr;
}

bool InitializeInterfaces()
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

		typedef IKeyValuesSystem* (*KeyValuesSystemFn)();
		KeyValuesSystemFn GetKeyValuesSystem = reinterpret_cast<KeyValuesSystemFn>(dlsym(vstdlib, "KeyValuesSystem"));

		if (!GetKeyValuesSystem)
			return false;

		interfaces::KeyValuesSystem = GetKeyValuesSystem();

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
		/*
                undefined FUN_0178d6c0()
                FUN_0178d6c0                         XREF[4]:     FUN_016e9160:016e91a8(c), 
                                                                	FUN_0186c670:0186c6b1(c), 
                                                                	02465b6c, 0252ea8c(*)  
        0178d6c0 4C 8D 15        LEA        R10,[gpGlobals]                                  = 02ee4c00
                 79 4B 70 01
        0178d6c7 49 8B 02        MOV        RAX,qword ptr [R10]=>gpGlobals                   = 02ee4c00
		*/
		uintptr_t unkFunc = reinterpret_cast<uintptr_t>(sigscan_module("client.so", "4C 8D 15 ? ? ? ? 49 8B 02"));
		interfaces::GlobalVars = *reinterpret_cast<CGlobalVars**>(vtable::ResolveRIP(unkFunc, 3, 7));
		
		if (!interfaces::GlobalVars)
			return false;
	}

	{ // attribute hook manager
		// get the function
		AttributeHookValue = reinterpret_cast<AttributeHookValueFn>(sigscan_module("client.so", "55 31 C0 48 89 E5 41 57 41 56 41 55 49 89 F5 41 54 49 89 FC 53 89 CB"));
		if (AttributeHookValue == nullptr)
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