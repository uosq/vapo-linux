#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_main.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <dlfcn.h>
#include "../sdk/interfaces/interfaces.h"
#include "../libdetour/libdetour.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl2.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "GL/glew.h"

#include "../settings.h"
#include "../gui/gui.h"
#include "../features/esp/esp.h"
#include "../features/lua/hooks.h"

using SwapWindowFn = void(*)(SDL_Window* window);
static SwapWindowFn original_SwapWindow = nullptr;
DETOUR_DECL_TYPE(void, original_SwapWindow, SDL_Window* window);

using PollEventFn = int(*)(SDL_Event* event);
static PollEventFn original_PollEvent = nullptr;
DETOUR_DECL_TYPE(int, original_PollEvent, SDL_Event* event);

using GetWindowSizeFn = int(*)(SDL_Window* window, int* w, int* h);
static GetWindowSizeFn original_GetWindowSize = nullptr;
DETOUR_DECL_TYPE(int, original_GetWindowSize, SDL_Window* window, int* w, int* h);

static detour_ctx_t swapdetour;
static detour_ctx_t polldetour;
static detour_ctx_t windowsizedetour;

static inline SDL_Window* tfwindow;

static void SetupImGuiStyle()
{
	// Fork of Rounded Visual Studio style from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();
	
	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 4.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 2.5f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 11.0f;
	style.ScrollbarRounding = 2.5f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 2.0f;
	style.TabRounding = 3.5f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
	
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5921569f, 0.5921569f, 0.5921569f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.41201305f, 0.41201568f, 0.41201717f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.5536425f, 0.55364627f, 0.55364805f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.32156864f, 0.32156864f, 0.33333334f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3529412f, 0.3529412f, 0.37254903f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3529412f, 0.3529412f, 0.37254903f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.59557945f, 0.91845495f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.32156864f, 0.32156864f, 0.33333334f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30980393f, 0.30980393f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.22745098f, 0.22745098f, 0.24705882f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
}

inline void Hooked_SwapWindow(SDL_Window* window)
{
	static SDL_GLContext origcontext = nullptr, ourcontext = nullptr;

	if (ourcontext == nullptr)
	{
		origcontext = SDL_GL_GetCurrentContext();
		ourcontext = SDL_GL_CreateContext(window);
		
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplOpenGL3_Init("#version 100");
		ImGui_ImplSDL2_InitForOpenGL(window, nullptr);    
		ImGuiIO& io = ImGui::GetIO();
		SetupImGuiStyle();
		io.ConfigWindowsMoveFromTitleBarOnly = true;
	}

	SDL_GL_MakeCurrent(window, ourcontext);

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

	ImGui_ImplOpenGL3_NewFrame();
  	ImGui_ImplSDL2_NewFrame();
  	ImGui::NewFrame();

	cursor = ImGui::GetMouseCursor();

	if (LuaHookManager::HasHooks("ImGui"))
		LuaHookManager::Call(Lua::m_luaState, "ImGui");

	if (settings.misc.spectatorlist)
		DrawSpectatorList();

	if (settings.misc.playerlist)
		DrawPlayerList();

	if (settings.menu_open)
		DrawMainWindow();

	ImGui::Render();
  	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  	SDL_GL_MakeCurrent(window, origcontext);

	DETOUR_ORIG_CALL(&swapdetour, original_SwapWindow, window);
}

inline int Hooked_PollEvent(SDL_Event* event)
{
	int ret;
	DETOUR_ORIG_GET(&polldetour, ret, original_PollEvent, event);

	if (!ret)
		return ret;

	if (tfwindow && ImGui::GetCurrentContext())
		ImGui_ImplSDL2_ProcessEvent(event);

	if (settings.menu_open)
		event->type = 0;

	return ret;
}

// Because of Vulkan, gotta get the window from this
// fuck my life
inline int Hooked_GetWindowSize(SDL_Window* window, int* w, int* h)
{
	tfwindow = window;

	int ret;
	DETOUR_ORIG_GET(&windowsizedetour, ret, original_GetWindowSize, tfwindow, w, h);
	return ret;
}

inline void HookSDL()
{
	void *sdl = dlopen("libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);
	if (sdl == nullptr)
		return interfaces::Cvar->ConsolePrintf("Couldn't load SDL2\n");

	original_SwapWindow = (SwapWindowFn)dlsym(sdl, "SDL_GL_SwapWindow");
	if (original_SwapWindow == nullptr)
		return interfaces::Cvar->ConsolePrintf("Couldn't get SwapWindow\n");

	original_PollEvent = (PollEventFn)dlsym(sdl, "SDL_PollEvent");
	if (original_PollEvent == nullptr)
		return interfaces::Cvar->ConsolePrintf("Couldn't get PollEvent\n");

	original_GetWindowSize = (GetWindowSizeFn)dlsym(sdl, "SDL_GetWindowSize");
	if (original_GetWindowSize == nullptr)
		return interfaces::Cvar->ConsolePrintf("Couldn't get GetWindowSize\n");

	detour_init(&swapdetour, (void*)original_SwapWindow, (void*)&Hooked_SwapWindow);
	if (!detour_enable(&swapdetour))
		return interfaces::Cvar->ConsolePrintf("Couldn't hook SwapWindow\n");

	detour_init(&polldetour, (void*)original_PollEvent, (void*)&Hooked_PollEvent);
	if (!detour_enable(&polldetour))
		return interfaces::Cvar->ConsolePrintf("Couldn't hook PollEvent\n");

	detour_init(&windowsizedetour, (void*)original_GetWindowSize, (void*)&Hooked_GetWindowSize);
	if (!detour_enable(&windowsizedetour))
		return interfaces::Cvar->ConsolePrintf("Couldn't hook GetWindowSize\n");

	interfaces::Cvar->ConsolePrintf("SDL2 hooked\n");
}