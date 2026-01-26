#include "settings.h"

Settings settings = {};
std::unordered_map<std::string, SettingEntry> g_SettingsMap = {};

#define REG_SETTING(path, field, type) \
    g_SettingsMap[#path "." #field] = { &settings.path.field, type };

// probably not very efficient code, but it works so fuck it ig
void RegisterSettings(void)
{
	// aimbot
	REG_SETTING(aimbot, enabled, SettingType::BOOL)
	REG_SETTING(aimbot, fov, SettingType::FLOAT)
	REG_SETTING(aimbot, key, SettingType::STRING)
	REG_SETTING(aimbot, autoshoot, SettingType::BOOL)
	REG_SETTING(aimbot, max_sim_time, SettingType::FLOAT)
	REG_SETTING(aimbot, viewmodelaim, SettingType::BOOL)
	REG_SETTING(aimbot, draw_fov_indicator, SettingType::BOOL)
	REG_SETTING(aimbot, ignorecloaked, SettingType::BOOL)
	REG_SETTING(aimbot, ignoreubered, SettingType::BOOL)
	REG_SETTING(aimbot, ignorehoovy, SettingType::BOOL)
	REG_SETTING(aimbot, ignorebonked, SettingType::BOOL)
	REG_SETTING(aimbot, melee, SettingType::INT)
	REG_SETTING(aimbot, waitforcharge, SettingType::BOOL)
	REG_SETTING(aimbot, mode, SettingType::INT)
	REG_SETTING(aimbot, smoothness, SettingType::FLOAT)

	// esp
	REG_SETTING(esp, enabled, SettingType::BOOL)
	REG_SETTING(esp, ignorecloaked, SettingType::BOOL)
	REG_SETTING(esp, buildings, SettingType::BOOL)
	REG_SETTING(esp, name, SettingType::BOOL)
	REG_SETTING(esp, box, SettingType::BOOL)
	REG_SETTING(esp, healthbar, SettingType::BOOL)
	REG_SETTING(esp, chams, SettingType::BOOL)
	REG_SETTING(esp, stencil, SettingType::INT)
	REG_SETTING(esp, blur, SettingType::INT)
	REG_SETTING(esp, weapon, SettingType::BOOL)

	// misc
	REG_SETTING(misc, thirdperson, SettingType::BOOL)
	REG_SETTING(misc, thirdperson_key, SettingType::STRING)
	REG_SETTING(misc, customfov_enabled, SettingType::BOOL)
	REG_SETTING(misc, customfov, SettingType::FLOAT)
	REG_SETTING(misc, spectatorlist, SettingType::BOOL)
	REG_SETTING(misc, backpack_expander, SettingType::BOOL)
	REG_SETTING(misc, sv_pure_bypass, SettingType::BOOL)
	REG_SETTING(misc, streamer_mode, SettingType::BOOL)
	REG_SETTING(misc, bhop, SettingType::BOOL)
	REG_SETTING(misc, accept_item_drop, SettingType::BOOL)
	REG_SETTING(misc, playerlist, SettingType::BOOL)
	REG_SETTING(misc, viewmodel_offset[0], SettingType::FLOAT)
	REG_SETTING(misc, viewmodel_offset[1], SettingType::FLOAT)
	REG_SETTING(misc, viewmodel_offset[2], SettingType::FLOAT)

	//triggerbot
	REG_SETTING(triggerbot, enabled, SettingType::BOOL)
	REG_SETTING(triggerbot, hitscan, SettingType::BOOL)
	REG_SETTING(triggerbot, autobackstab, SettingType::BOOL)
	REG_SETTING(triggerbot, key, SettingType::STRING)

	// colors
	REG_SETTING(colors, red_team[0], SettingType::INT)
	REG_SETTING(colors, red_team[1], SettingType::INT)
	REG_SETTING(colors, red_team[2], SettingType::INT)
	REG_SETTING(colors, red_team[3], SettingType::INT)
	REG_SETTING(colors, blu_team[0], SettingType::INT)
	REG_SETTING(colors, blu_team[1], SettingType::INT)
	REG_SETTING(colors, blu_team[2], SettingType::INT)
	REG_SETTING(colors, blu_team[3], SettingType::INT)
	REG_SETTING(colors, aimbot_target[0], SettingType::INT)
	REG_SETTING(colors, aimbot_target[1], SettingType::INT)
	REG_SETTING(colors, aimbot_target[2], SettingType::INT)
	REG_SETTING(colors, aimbot_target[3], SettingType::INT)
	REG_SETTING(colors, weapon[0], SettingType::INT)
	REG_SETTING(colors, weapon[1], SettingType::INT)
	REG_SETTING(colors, weapon[2], SettingType::INT)
	REG_SETTING(colors, weapon[3], SettingType::INT)

	// antiaim
	REG_SETTING(antiaim, enabled, SettingType::BOOL)
	REG_SETTING(antiaim, pitch_mode, SettingType::INT)
	REG_SETTING(antiaim, real_yaw_mode, SettingType::INT)
	REG_SETTING(antiaim, fake_yaw_mode, SettingType::INT)
	REG_SETTING(antiaim, spin_speed, SettingType::FLOAT)
}