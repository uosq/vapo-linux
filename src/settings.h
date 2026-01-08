#pragma once

#include <string>
#include <unistd.h>

struct Settings_ESP
{
	bool enabled = false;
	bool ignorecloaked = false;
	bool buildings = false;
	bool name = false;
	bool box = false;
	bool chams = false;
	int stencil = 0;
	int blur = 0;
};

struct Settings_Aimbot
{
	bool enabled = false;
	float fov = 0.0f;
	std::string key = "";
	bool autoshoot = false;
	float max_sim_time = 0.0f;
	bool viewmodelaim = false;
	bool draw_fov_indicator = false;
};

struct Settings_Misc
{
	bool thirdperson = false;
	std::string thirdperson_key = "";
	bool customfov_enabled = false;
	float customfov = 0.0f;
	bool spectatorlist = false;
	bool backpack_expander = false;
	bool sv_pure_bypass = false;
	bool streamer_mode = false;
	bool bhop = false;

	float viewmodel_offset[3] = {0.0, 0.0, 0.0};
};

struct Settings
{
	Settings_ESP esp;
	Settings_Aimbot aimbot;
	Settings_Misc misc;

	bool menu_open = false;
};

static Settings settings;