#pragma once

#include <string>
#include <unistd.h>

enum class PitchMode
{
	NONE = 0,
	UP, DOWN,
	FAKEUP, FAKEDOWN,
	RANDOM,
};

enum class YawMode
{
	NONE = 0,
	LEFT, RIGHT,
	BACK, FORWARD,
	SPIN_LEFT, SPIN_RIGHT,
	JITTER,
};

struct Settings_Antiaim
{
	bool enabled = false;
	PitchMode pitch_mode = PitchMode::NONE;
	YawMode real_yaw_mode = YawMode::NONE;
	YawMode fake_yaw_mode = YawMode::NONE;
};

struct Settings_ESP
{
	bool enabled = false;
	bool ignorecloaked = false;
	bool buildings = false;
	bool name = false;
	bool box = false;
	bool healthbar = false;
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
	bool accept_item_drop = false;

	float viewmodel_offset[3] = {0.0, 0.0, 0.0};
};

struct Settings_Triggerbot
{
	bool enabled = false;
	bool hitscan = false;
	bool autobackstab = false;
	std::string key = "";
};

struct Settings
{
	Settings_Misc misc;
	Settings_Aimbot aimbot;
	Settings_ESP esp;
	Settings_Triggerbot triggerbot;
	Settings_Antiaim antiaim;

	bool menu_open = false;
};

static Settings settings;