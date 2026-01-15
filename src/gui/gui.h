#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../settings.h"
#include "../sdk/helpers/helper.h"
#include "../features/antiaim/antiaim.h"
#include "../imgui/TextEditor.h"
#include "../features/lua/api.h"
#include "console.h"

static TextEditor editor;

enum TabMenu
{
	TAB_AIMBOT = 0,
	TAB_ESP,
	TAB_MISC,
	TAB_TRIGGER,
	TAB_ANTIAIM,
	TAB_LUA,
};

static void DrawTabButtons(int &tab)
{
	ImGui::BeginGroup();

	if (ImGui::Button("Aimbot", ImVec2(-1, 0)))
		tab = TAB_AIMBOT;

	if (ImGui::Button("Trigger", ImVec2(-1, 0)))
		tab = TAB_TRIGGER;

	if (ImGui::Button("ESP", ImVec2(-1, 0)))
		tab = TAB_ESP;

	if (ImGui::Button("Misc", ImVec2(-1, 0)))
		tab = TAB_MISC;

	if (ImGui::Button("Antiaim", ImVec2(-1, 0)))
		tab = TAB_ANTIAIM;

	if (ImGui::Button("Lua", ImVec2(-1, 0)))
		tab = TAB_LUA;

	ImGui::EndGroup();
}

static void DrawAimbotTab()
{
	ImGui::BeginGroup();

	ImGui::Checkbox("Enabled", &settings.aimbot.enabled);
	ImGui::InputText("Key", &settings.aimbot.key);
	ImGui::Checkbox("Autoshoot", &settings.aimbot.autoshoot);
	ImGui::Checkbox("Viewmodel Aim", &settings.aimbot.viewmodelaim);
	ImGui::Checkbox("Draw FOV Indicator", &settings.aimbot.draw_fov_indicator);
	ImGui::SliderFloat("Fov", &settings.aimbot.fov, 0.0f, 180.0f);
	ImGui::SliderFloat("Max Sim Time", &settings.aimbot.max_sim_time, 0.0f, 5.0f);

	ImGui::EndGroup();
}

static void DrawESPTab()
{
	ImGui::BeginGroup();

	ImGui::Checkbox("ESP Enabled", &settings.esp.enabled);
	ImGui::Checkbox("Name", &settings.esp.name);
	ImGui::Checkbox("Box", &settings.esp.box);
	ImGui::Checkbox("Ignore Cloakeds", &settings.esp.ignorecloaked);
	ImGui::Checkbox("Buildings", &settings.esp.buildings);

	ImGui::Separator();

	ImGui::Checkbox("Chams", &settings.esp.chams);

	ImGui::Separator();

	ImGui::TextUnformatted("Glow");
	ImGui::SliderInt("Stencil", &settings.esp.stencil, 0, 10);
	ImGui::SliderInt("Blur", &settings.esp.blur, 0, 10);

	ImGui::EndGroup();
}

static void DrawMiscTab()
{
	ImGui::BeginGroup();

	ImGui::Checkbox("Third person", &settings.misc.thirdperson);
	ImGui::InputText("Third Person Key", &settings.misc.thirdperson_key);

	ImGui::Separator();

	ImGui::Checkbox("Spectator List", &settings.misc.spectatorlist);
	ImGui::Checkbox("sv_pure bypass", &settings.misc.sv_pure_bypass);
	ImGui::Checkbox("Streamer Mode", &settings.misc.streamer_mode);
	ImGui::Checkbox("Bhop", &settings.misc.bhop);
	ImGui::Checkbox("Backpack Expander", &settings.misc.backpack_expander);

	ImGui::Separator();

	ImGui::Checkbox("Custom Fov Enabled", &settings.misc.customfov_enabled);
	ImGui::SliderFloat("Custom Fov", &settings.misc.customfov, 0, 140);

	ImGui::Separator();

	ImGui::SliderFloat3("Viewmodel Offset", settings.misc.viewmodel_offset, -20, 20.0f );

	ImGui::Separator();

	ImGui::Checkbox("Accept Item Drops", &settings.misc.accept_item_drop);

	ImGui::EndGroup();
}

static void DrawTriggerTab()
{
	ImGui::BeginGroup();

	ImGui::Checkbox("Trigger Enabled", &settings.triggerbot.enabled);
	ImGui::InputText("Trigger Key", &settings.triggerbot.key);
	ImGui::Checkbox("Hitscan", &settings.triggerbot.hitscan);
	ImGui::Checkbox("Auto Backstab", &settings.triggerbot.autobackstab);

	ImGui::EndGroup();
}

static void DrawAntiaimTab()
{
	ImGui::BeginGroup();

	ImGui::Checkbox("Enabled", &settings.antiaim.enabled);

	if (ImGui::BeginCombo("Pitch Mode", Antiaim::GetPitchModeName(settings.antiaim.pitch_mode).c_str()))
	{
		if (ImGui::Selectable("None"))
			settings.antiaim.pitch_mode = PitchMode::NONE;

		if (ImGui::Selectable("Up"))
			settings.antiaim.pitch_mode = PitchMode::UP;

		if (ImGui::Selectable("Down"))
			settings.antiaim.pitch_mode = PitchMode::DOWN;

		if (ImGui::Selectable("Fake Up"))
			settings.antiaim.pitch_mode = PitchMode::FAKEUP;

		if (ImGui::Selectable("Fake Down"))
			settings.antiaim.pitch_mode = PitchMode::FAKEDOWN;

		if (ImGui::Selectable("Random"))
			settings.antiaim.pitch_mode = PitchMode::RANDOM;

		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Real Yaw Mode", Antiaim::GetYawModeName(settings.antiaim.real_yaw_mode).c_str()))
	{
		if (ImGui::Selectable("None"))
			settings.antiaim.real_yaw_mode = YawMode::NONE;

		if (ImGui::Selectable("Left"))
			settings.antiaim.real_yaw_mode = YawMode::LEFT;

		if (ImGui::Selectable("Right"))
			settings.antiaim.real_yaw_mode = YawMode::RIGHT;

		if (ImGui::Selectable("Spin Left"))
			settings.antiaim.real_yaw_mode = YawMode::SPIN_LEFT;

		if (ImGui::Selectable("Spin Right"))
			settings.antiaim.real_yaw_mode = YawMode::SPIN_RIGHT;

		if (ImGui::Selectable("Jitter"))
			settings.antiaim.real_yaw_mode = YawMode::JITTER;

		if (ImGui::Selectable("Back"))
			settings.antiaim.real_yaw_mode = YawMode::BACK;

		if (ImGui::Selectable("Forward"))
			settings.antiaim.real_yaw_mode = YawMode::FORWARD;

		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Fake Yaw Mode", Antiaim::GetYawModeName(settings.antiaim.fake_yaw_mode).c_str()))
	{
		if (ImGui::Selectable("None"))
			settings.antiaim.fake_yaw_mode = YawMode::NONE;

		if (ImGui::Selectable("Left"))
			settings.antiaim.fake_yaw_mode = YawMode::LEFT;

		if (ImGui::Selectable("Right"))
			settings.antiaim.fake_yaw_mode = YawMode::RIGHT;

		if (ImGui::Selectable("Spin Left"))
			settings.antiaim.fake_yaw_mode = YawMode::SPIN_LEFT;

		if (ImGui::Selectable("Spin Right"))
			settings.antiaim.fake_yaw_mode = YawMode::SPIN_RIGHT;

		if (ImGui::Selectable("Jitter"))
			settings.antiaim.fake_yaw_mode = YawMode::JITTER;

		if (ImGui::Selectable("Back"))
			settings.antiaim.fake_yaw_mode = YawMode::BACK;

		if (ImGui::Selectable("Forward"))
			settings.antiaim.fake_yaw_mode = YawMode::FORWARD;

		ImGui::EndCombo();
	}

	ImGui::EndGroup();
}

static void DrawLuaTab()
{
	static bool init = false;
	if (!init)
	{
		editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
		editor.SetPalette(TextEditor::GetDarkPalette());
		editor.SetShowWhitespaces(false);
		init = true;
	}

	ImGui::BeginGroup();

	if (ImGui::BeginTabBar("LuaTab"))
	{
		if (ImGui::BeginTabItem("Console Tab"))
		{
			ImGui::InputTextMultiline(
				"##ConsoleText",
				&consoleText,
				ImGui::GetContentRegionAvail(),
				ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap
			);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Editor Tab"))
		{
			editor.Render("Editor", ImVec2(0, -25));
	
			ImGui::Spacing();
	
			if (ImGui::Button("Run Code"))
				Lua::RunCode(editor.GetText());
	
			ImGui::SameLine();
	
			if (ImGui::Button("Clear"))
				editor.SetText("");

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::EndGroup();
}

static void DrawSpectatorList()
{
	if (helper::engine::IsTakingScreenshot())
		return;

	ImGui::SetNextWindowSizeConstraints(
        	ImVec2(150.0f, 0.0f),
        	ImVec2(FLT_MAX, FLT_MAX)
    	);

	int flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	if (!settings.menu_open)
		flags |= ImGuiWindowFlags_NoMove;

	ImGui::Begin("Spectator List", nullptr, flags);

	int maxclients = helper::engine::GetMaxClients();
	if (!helper::engine::IsInMatch() || maxclients <= 1)
		return ImGui::End();

	CTFPlayer* pLocal = helper::engine::GetLocalPlayer();
	if (pLocal == nullptr || !pLocal->IsAlive())
		return ImGui::End();

	int localTeam = pLocal->m_iTeamNum();
	int localIndex = pLocal->GetIndex();

	for (int i = 1; i < maxclients; i++)
	{
		CTFPlayer* player = static_cast<CTFPlayer*>(interfaces::EntityList->GetClientEntity(i));
		if (player == nullptr || !player->IsPlayer() || player->IsAlive() || player == pLocal)
			continue;

		if (player->m_iTeamNum() != localTeam)
			continue;

		CTFPlayer* m_hObserverTarget = HandleAs<CTFPlayer*>(player->m_hObserverTarget());
		if (!m_hObserverTarget || m_hObserverTarget->GetIndex() != localIndex)
			continue;

		player_info_t info;
		if (!interfaces::Engine->GetPlayerInfo(i, &info))
			continue;

		int m_iObserverMode = player->m_iObserverMode();
		bool isfirstperson = m_iObserverMode == OBS_MODE_IN_EYE;

		ImGui::TextColored(isfirstperson ? ImVec4(1.0, 0.5, 0.5, 1.0) : ImVec4(1.0, 1.0, 1.0, 1.0), "%s", player->GetName().c_str());
	}

	ImGui::End();
}

static void DrawMainWindow()
{
	if (helper::engine::IsTakingScreenshot())
		return;

	static int tab = 0;

	if (ImGui::Begin("Vapo Linux", nullptr, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::BeginTable("MainTable", 2, 0))
		{
			ImGui::TableSetupColumn("Buttons", ImGuiTableColumnFlags_WidthFixed, 70);
			ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);
			
			ImGui::TableNextRow();
			
			ImGui::TableSetColumnIndex(0);
			DrawTabButtons(tab);

			ImGui::TableSetColumnIndex(1);
			switch(tab)
			{
				case TAB_AIMBOT: DrawAimbotTab(); break;
				case TAB_ESP: DrawESPTab(); break;
				case TAB_MISC: DrawMiscTab(); break;
				case TAB_TRIGGER: DrawTriggerTab(); break;
				case TAB_ANTIAIM: DrawAntiaimTab(); break;
				case TAB_LUA: DrawLuaTab(); break;
				default: break;
			}
			
			ImGui::EndTable();
		}
	}
	ImGui::End();
}