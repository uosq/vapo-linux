#include <dlfcn.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_sdl2.h"
#include "../sdk/interfaces/interfaces.h"
#include "../gui/gui.h"
#include "sdl.h"
#include "../libdetour/libdetour.h"
#include "../sdk/definitions/d3d9.h"

typedef struct IDirect3DDevice9 *LPDIRECT3DDEVICE9;

static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS g_d3dpp = {};
static bool g_ImGuiInitialized = false;

typedef HRESULT(__stdcall* Present_t)(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
typedef HRESULT(__stdcall* Reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

static Present_t original_Present = nullptr;
static Reset_t original_Reset = nullptr;

DETOUR_DECL_TYPE(HRESULT, original_Present, IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
DETOUR_DECL_TYPE(HRESULT, original_Reset, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

static detour_ctx_t present_ctx;
static detour_ctx_t reset_ctx;

static void InitImGui()
{
	if (g_ImGuiInitialized || !g_pd3dDevice)
		return;

	if (!tfwindow) return interfaces::Cvar->ConsolePrintf("SDL window is NULL!\n");

	if (ImGui::GetCurrentContext() == nullptr)
		ImGui::CreateContext();

	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

	//if (!ImGui_ImplSDL2_InitForD3D(tfwindow)) {
	if (!ImGui_ImplSDL2_InitForVulkan(tfwindow))
	{
		interfaces::Cvar->ConsolePrintf("ImGui_ImplSDL2_InitForD3D failed!\n");
		return;
	}
	
	ImGui_ImplDX9_Init(g_pd3dDevice);
	SetupImGuiStyle();
	
	g_ImGuiInitialized = true;
}

static void CleanupImGui()
{
	if (!g_ImGuiInitialized)
		return;

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	
	g_ImGuiInitialized = false;
}

static D3DFORMAT GetBackBufferFormat(IDirect3DDevice9* device)
{
	IDirect3DSurface9* pBackBuffer = nullptr;
	device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	
	if (pBackBuffer)
	{
		D3DSURFACE_DESC desc;
		pBackBuffer->GetDesc(&desc);
		pBackBuffer->Release();
		
		//interfaces::Cvar->ConsolePrintf("Backbuffer format: %d\n", desc.Format);
		return desc.Format;
	}
	
	return D3DFMT_UNKNOWN;
}

static void RenderImGui()
{
	if (!g_pd3dDevice || !g_ImGuiInitialized)
		return;

	static bool checkedFormat = false;
	static bool needsGammaCorrection = false;
	
	if (!checkedFormat)
	{
		D3DFORMAT format = GetBackBufferFormat(g_pd3dDevice);
		// If using sRGB, we need gamma correction
		needsGammaCorrection = (format == 22 || format == 21); // Common sRGB formats
		checkedFormat = true;
		
		//interfaces::Cvar->ConsolePrintf("Gamma correction: %s\n", needsGammaCorrection ? "enabled" : "disabled");
	}

	// Disable sRGB writes temporarily if needed
	if (needsGammaCorrection)
		g_pd3dDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (ImGui::IsKeyPressed(ImGuiKey_Insert, false) || ImGui::IsKeyPressed(ImGuiKey_F11, false))
	{
		settings.menu_open = !settings.menu_open;
		interfaces::Surface->SetCursorAlwaysVisible(settings.menu_open);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
	{
		settings.menu_open = false;
		interfaces::Surface->SetCursorAlwaysVisible(settings.menu_open);
	}

	cursor = ImGui::GetMouseCursor();

	if (LuaHookManager::HasHooks("ImGui"))
		LuaHookManager::Call(Lua::m_luaState, "ImGui");

	if (settings.misc.spectatorlist)
		DrawSpectatorList();

	if (settings.misc.playerlist)
		DrawPlayerList();

	if (settings.menu_open)
		DrawMainWindow();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	if (needsGammaCorrection)
		g_pd3dDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);
}

static HRESULT __stdcall Hooked_Present(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	if (!g_pd3dDevice)
		g_pd3dDevice = pDevice;

	InitImGui();
	RenderImGui();

	HRESULT ret;
	DETOUR_ORIG_GET(&present_ctx, ret, original_Present, pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	return ret;
}

static HRESULT __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	// ImGui needs to be cleaned up before device reset
	CleanupImGui();

	HRESULT ret;
	DETOUR_ORIG_GET(&reset_ctx, ret, original_Reset, pDevice, pPresentationParameters);

	if (SUCCEEDED(ret))
	{
		g_d3dpp = *pPresentationParameters;
		// ImGui will be reinitialized on next Present call
	}

	return ret;
}

static void* GetModuleBaseAddress(const char* module_name)
{
	std::ifstream file("/proc/self/maps");
	if (!file.is_open())
		return nullptr;

	std::string line;
	while (std::getline(file, line))
	{
		if (line.find(module_name) != std::string::npos)
		{
			size_t dash_pos = line.find('-');
			if (dash_pos != std::string::npos)
			{
				std::string addr_str = line.substr(0, dash_pos);
				return (void*)std::stoull(addr_str, nullptr, 16);
			}
		}
	}

	return nullptr;
}

static bool HookD3D9VTable()
{
	// DXVK creates a D3D9 device, we need to get the vtable
	void* dxvk_base = GetModuleBaseAddress("libdxvk_d3d9.so");
	if (!dxvk_base)
	{
		interfaces::Cvar->ConsolePrintf("DXVK not found\n");
		return false;
	}

	interfaces::Cvar->ConsolePrintf("DXVK base: %p\n", dxvk_base);

	// Create a temporary D3D9 device to get the vtable
	void* d3d9_lib = dlopen("libdxvk_d3d9.so", RTLD_LAZY | RTLD_NOLOAD);
	if (!d3d9_lib)
	{
		interfaces::Cvar->ConsolePrintf("Could not load libdxvk_d3d9.so\n");
		return false;
	}

	typedef IDirect3D9* (__stdcall* Direct3DCreate9_t)(UINT);
	Direct3DCreate9_t pDirect3DCreate9 = (Direct3DCreate9_t)dlsym(d3d9_lib, "Direct3DCreate9");
	
	if (!pDirect3DCreate9)
	{
		interfaces::Cvar->ConsolePrintf("Could not find Direct3DCreate9\n");
		return false;
	}

	IDirect3D9* pD3D = pDirect3DCreate9(D3D_SDK_VERSION);
	if (!pD3D)
	{
		interfaces::Cvar->ConsolePrintf("Direct3DCreate9 failed\n");
		return false;
	}

	// Create a dummy device to get vtable
	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth = 1;
	d3dpp.BackBufferHeight = 1;
	d3dpp.hDeviceWindow = nullptr; // DXVK doesnt require this

	IDirect3DDevice9* pDummyDevice = nullptr;
	HRESULT hr = pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_NULLREF, // Use null reference device for dummy
		nullptr,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&pDummyDevice
	);

	if (FAILED(hr) || !pDummyDevice)
	{
		interfaces::Cvar->ConsolePrintf("Failed to create dummy device (0x%X)\n", hr);
		pD3D->Release();
		return false;
	}

	void** vtable = *(void***)pDummyDevice;

	original_Present = (Present_t)vtable[17];
	original_Reset = (Reset_t)vtable[16];
	//original_EndScene = (EndScene_t)vtable[42];

	//interfaces::Cvar->ConsolePrintf("D3D9 VTable:\n");
	//interfaces::Cvar->ConsolePrintf("Present:%p\n", original_Present);
	//interfaces::Cvar->ConsolePrintf("Reset:%p\n", original_Reset);

	detour_init(&present_ctx, (void*)original_Present, (void*)&Hooked_Present);
	if (!detour_enable(&present_ctx))
	{
		interfaces::Cvar->ConsolePrintf("Failed to hook Present\n");
		pDummyDevice->Release();
		pD3D->Release();
		return false;
	}

	detour_init(&reset_ctx, (void*)original_Reset, (void*)&Hooked_Reset);
	if (!detour_enable(&reset_ctx))
	{
		interfaces::Cvar->ConsolePrintf("Failed to hook Reset\n");
		detour_disable(&present_ctx);
		pDummyDevice->Release();
		pD3D->Release();
		return false;
	}

	// Cleanup dummy resources
	pDummyDevice->Release();
	pD3D->Release();

	interfaces::Cvar->ConsolePrintf("D3D9/DXVK hooked successfully\n");
	return true;
}

static void HookDXVK()
{
	if (GetModuleBaseAddress("libdxvk_d3d9.so") == nullptr)
		return interfaces::Cvar->ConsolePrintf("DXVK not loaded\n");

	if (!HookD3D9VTable())
		return interfaces::Cvar->ConsolePrintf("Failed to hook DXVK\n");
}