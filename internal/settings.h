#pragma once

#include "json.hpp"
#include "sdk/definitions/buttoncode.h"

struct Settings_ESP
{
	bool enabled = true;
	bool ignorecloaked = true;
};

struct Settings_Aimbot
{
	bool enabled = true;
	float fov = 30.0f;
	std::string key = "shift";
	bool autoshoot = true;
	float max_sim_time = 2.0f;
};

struct Settings_Misc
{
	bool thirdperson = false;
	float customfov = 90.0f;
};

struct Settings
{
	Settings_ESP esp;
	Settings_Aimbot aimbot;
	Settings_Misc misc;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_Misc, thirdperson, customfov)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_ESP, enabled, ignorecloaked)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_Aimbot, enabled, fov, key, autoshoot, max_sim_time)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, esp, aimbot, misc)

static inline Settings settings;