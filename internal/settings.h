#pragma once

#include "httplib.h"
#include "json.hpp"
#include "sdk/definitions/convar.h"
#include <atomic>
#include <pthread.h>
#include <string>
#include <unistd.h>

struct Settings_ESP
{
	bool enabled = true;
	bool ignorecloaked = true;
	bool buildings = true;
	bool name = true;
	bool box = true;
	bool chams = true;
};

struct Settings_Aimbot
{
	bool enabled = true;
	float fov = 30.0f;
	std::string key = "shift";
	bool autoshoot = true;
	float max_sim_time = 2.0f;
	bool viewmodelaim = true;
};

struct Settings_Misc
{
	bool thirdperson = false;
	bool customfov_enabled = false;
	float customfov = 0.0f;
	bool spectatorlist = true;
	bool backpack_expander = true;
	bool sv_pure_bypass = true;
	bool streamer_mode = true;
};

struct Settings
{
	Settings_ESP esp;
	Settings_Aimbot aimbot;
	Settings_Misc misc;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_Misc, thirdperson, customfov_enabled, customfov, spectatorlist, backpack_expander, sv_pure_bypass, streamer_mode)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_ESP, enabled, ignorecloaked, buildings, name, box, chams)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_Aimbot, enabled, fov, key, autoshoot, max_sim_time, viewmodelaim)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, esp, aimbot, misc)

static inline Settings settings;
static inline httplib::Client cli("http://127.0.0.1:6969");