#pragma once

#include "json.hpp"

struct Settings_ESP
{
	bool enabled = true;
	bool ignorecloaked = true;
};

struct Settings_Aimbot
{
	bool enabled = true;
	float fov = 30.0f;
	std::string key = "lshift";
};

struct Settings
{
	Settings_ESP esp;
	Settings_Aimbot aimbot;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_ESP, enabled)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_Aimbot, enabled, fov, key)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, esp, aimbot)

static inline Settings settings;