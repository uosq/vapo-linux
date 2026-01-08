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

inline void Hooked_SwapWindow(SDL_Window* window)
{
	static SDL_GLContext origcontext = nullptr, ourcontext = nullptr;

	if (ourcontext == nullptr)
	{
		origcontext = SDL_GL_GetCurrentContext();
		ourcontext = SDL_GL_CreateContext(window);

		GLenum err = glewInit();
		if (err != GLEW_OK) {
			interfaces::Cvar->ConsolePrintf("Failed to initialize GLEW: %s\n", glewGetErrorString(err));
			DETOUR_ORIG_CALL(&swapdetour, original_SwapWindow, window);
			return;
		}
		
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplOpenGL3_Init("#version 100");
		ImGui_ImplSDL2_InitForOpenGL(window, nullptr);    
		ImGuiIO& io = ImGui::GetIO();
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

	if (settings.misc.spectatorlist)
		DrawSpectatorList();

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