#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../settings.h"
#include "../sdk/helpers/helper.h"
#include "../features/antiaim/antiaim.h"
#include "../imgui/TextEditor.h"
#include "../features/lua/api.h"
#include "console.h"
#include "netvar_parser.h"
#include "../features/entitylist/entitylist.h"
#include "../sdk/definitions/eteam.h"
#include "../features/aimbot/melee/aimbot_melee.h"
#include "../features/aimbot/utils/utils.h"

static TextEditor editor;

enum TabMenu
{
	TAB_AIMBOT = 0,
	TAB_ESP,
	TAB_MISC,
	TAB_TRIGGER,
	TAB_ANTIAIM,
	TAB_LUA,
	TAB_NETVARS,
};

static void DrawTabButtons(int &tab)
{
	ImGui::BeginGroup();

	if (ImGui::Button("AIMBOT", ImVec2(-1, 0)))
		tab = TAB_AIMBOT;

	if (ImGui::Button("TRIGGER", ImVec2(-1, 0)))
		tab = TAB_TRIGGER;

	if (ImGui::Button("ESP", ImVec2(-1, 0)))
		tab = TAB_ESP;

	if (ImGui::Button("MISC", ImVec2(-1, 0)))
		tab = TAB_MISC;

	if (ImGui::Button("ANTIAIM", ImVec2(-1, 0)))
		tab = TAB_ANTIAIM;

	if (ImGui::Button("PLUTO", ImVec2(-1, 0)))
		tab = TAB_LUA;

	if (ImGui::Button("NETVARS", ImVec2(-1, 0)))
		tab = TAB_NETVARS;

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
	ImGui::Checkbox("Wait For Charge", &settings.aimbot.waitforcharge);
	ImGui::SliderFloat("Fov", &settings.aimbot.fov, 0.0f, 180.0f);
	ImGui::SliderFloat("Max Sim Time", &settings.aimbot.max_sim_time, 0.0f, 5.0f);
	ImGui::SliderFloat("Smoothness", &settings.aimbot.smoothness, 10.0f, 100.0f);

	if (ImGui::BeginCombo("Melee Aimbot", GetMeleeModeName().c_str()))
	{
		if (ImGui::Selectable("None"))
			settings.aimbot.melee = MeleeMode::NONE;

		if (ImGui::Selectable("Legit"))
			settings.aimbot.melee = MeleeMode::LEGIT;

		if (ImGui::Selectable("Rage"))
			settings.aimbot.melee = MeleeMode::RAGE;

		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Aimbot Method", AimbotUtils::GetAimbotModeName().c_str()))
	{
		if (ImGui::Selectable("Plain"))
			settings.aimbot.mode = AimbotMode::PLAIN;

		if (ImGui::Selectable("Smooth"))
			settings.aimbot.mode = AimbotMode::SMOOTH;

		if (ImGui::Selectable("Assistance"))
			settings.aimbot.mode = AimbotMode::ASSISTANCE;

		if (ImGui::Selectable("Silent"))
			settings.aimbot.mode = AimbotMode::SILENT;

		if (ImGui::Selectable("pSilent"))
			settings.aimbot.mode = AimbotMode::PSILENT;

		ImGui::EndCombo();
	}

	ImGui::Separator();

	ImGui::TextUnformatted("Ignore Options");
	ImGui::Checkbox("Cloaked", &settings.aimbot.ignorecloaked);
	ImGui::SameLine();
	ImGui::Checkbox("Ubercharged", &settings.aimbot.ignoreubered);
	ImGui::SameLine();
	ImGui::Checkbox("Hoovy", &settings.aimbot.ignorehoovy);
	ImGui::SameLine();
	ImGui::Checkbox("Bonked", &settings.aimbot.ignorebonked);

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
	ImGui::Checkbox("Weapon", &settings.esp.weapon);

	ImGui::Separator();

	ImGui::Checkbox("Chams", &settings.esp.chams);

	ImGui::Separator();

	ImGui::TextUnformatted("Glow");
	ImGui::SliderInt("Stencil", &settings.esp.stencil, 0, 10);
	ImGui::SliderInt("Blur", &settings.esp.blur, 0, 10);

	ImGui::Separator();

	static float red[3] = {settings.colors.red_team.r()/255.0f, settings.colors.red_team.g()/255.0f, settings.colors.red_team.b()/255.0f};
	static float blu[3] = {settings.colors.blu_team.r()/255.0f, settings.colors.blu_team.g()/255.0f, settings.colors.blu_team.b()/255.0f};
	static float target[3] = {settings.colors.aimbot_target.r()/255.0f, settings.colors.aimbot_target.g()/255.0f, settings.colors.aimbot_target.b()/255.0f};
	static float weapon[3] = {settings.colors.weapon.r()/255.0f, settings.colors.weapon.g()/255.0f, settings.colors.weapon.b()/255.0f};

	ImGui::TextUnformatted("Colors");

	if (ImGui::ColorEdit3("RED Team", red))
		settings.colors.red_team.SetColor(red[0]*255.0f, red[1]*255.0f, red[2]*255.0f, 255.0f);

	if (ImGui::ColorEdit3("BLU Team", blu))
		settings.colors.blu_team.SetColor(blu[0]*255.0f, blu[1]*255.0f, blu[2]*255.0f, 255.0f);

	if (ImGui::ColorEdit3("Aimbot Target", target))
		settings.colors.aimbot_target.SetColor(target[0]*255.0f, target[1]*255.0f, target[2]*255.0f, 255.0f);

	if (ImGui::ColorEdit3("Weapon", weapon))
		settings.colors.weapon.SetColor(weapon[0]*255.0f, weapon[1]*255.0f, weapon[2]*255.0f, 255.0f);

	ImGui::EndGroup();
}

static void DrawMiscTab()
{
	ImGui::BeginGroup();

	ImGui::Checkbox("Third person", &settings.misc.thirdperson);
	ImGui::InputText("Third Person Key", &settings.misc.thirdperson_key);

	ImGui::Separator();

	ImGui::Checkbox("Spectator List", &settings.misc.spectatorlist);
	ImGui::Checkbox("Player List", &settings.misc.playerlist);
	ImGui::Checkbox("sv_pure bypass", &settings.misc.sv_pure_bypass);
	ImGui::Checkbox("Streamer Mode", &settings.misc.streamer_mode);
	ImGui::Checkbox("Bhop", &settings.misc.bhop);
	ImGui::Checkbox("Backpack Expander", &settings.misc.backpack_expander);
	ImGui::Checkbox("Accept Item Drops", &settings.misc.accept_item_drop);

	ImGui::Separator();

	ImGui::Checkbox("Custom Fov Enabled", &settings.misc.customfov_enabled);
	ImGui::SliderFloat("Custom Fov", &settings.misc.customfov, 0, 140);

	ImGui::Separator();

	ImGui::SliderFloat3("Viewmodel Offset", settings.misc.viewmodel_offset, -20, 20.0f );

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

	ImGui::SliderFloat("Spin Speed", &settings.antiaim.spin_speed, 0.0f, 10.0f);

	ImGui::EndGroup();
}

static void DrawLuaTab()
{
	static bool init = false;
	if (!init)
	{
		const char* keywords[]
		{
			"begin", "switch", "continue", "number",
			"int", "float", "boolean", "bool",
			"function", "table", "userdata", "nil",
			"any", "void", "enum", "export",
			"class", "new", "parent", "try",
			"catch",
		};

		const char* myIdentifiers[]
		{
			"globals", "engine",
			"hooks", "Vector3",
			"entities", "draw",
			"render", "materials",
			"client", "BitBuffer",
			"clientstate", "ui",
			"menu", "aimbot"
		};

		auto def = TextEditor::LanguageDefinition::Lua();
		def.mPreprocChar = '$';

		for (auto& k: keywords)
			def.mKeywords.insert(k);

		TextEditor::Identifier id;
		id.mDeclaration = "Custom Library";

		for (auto& k : myIdentifiers)
			def.mIdentifiers.insert(std::make_pair(std::string(k), id));

		editor.SetLanguageDefinition(def);
		editor.SetPalette(TextEditor::GetDarkPalette());
		editor.SetShowWhitespaces(false);
		init = true;
	}

	ImGui::BeginGroup();

	if (ImGui::BeginTabBar("LuaTab"))
	{
		if (ImGui::BeginTabItem("Console Tab"))
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			size.y -= 30;

			ImGui::InputTextMultiline(
				"##ConsoleText",
				&consoleText,
				size,
				ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap
			);

			ImGui::Spacing();

			if (ImGui::Button("Clear"))
				consoleText = "";

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

static void DrawParsedNetvarData(const std::vector<ClassEntry>& classes)
{
	for (const auto& cls : classes)
	{
		if (ImGui::TreeNode(cls.className.c_str()))
		{
			if (cls.members.empty())
				ImGui::TextDisabled("No members");
			else
				for (const auto& name : cls.members)
					ImGui::BulletText("%s", name.c_str());

			ImGui::TreePop();
		}
	}
}

static void DrawNetVarsTab()
{
	static auto path = GetLocalFilePath("netvars.txt");
	static auto data = ParseFile(path);

	if (ImGui::BeginChild("NetvarContent"))
		DrawParsedNetvarData(data);
	ImGui::EndChild();

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

	for (const auto& player : EntityList::m_vecPlayers)
	{
		if (player->IsAlive() || player == pLocal)
			continue;

		if (player->m_iTeamNum() != localTeam)
			continue;

		CTFPlayer* m_hObserverTarget = HandleAs<CTFPlayer*>(player->m_hObserverTarget());
		if (!m_hObserverTarget || m_hObserverTarget->GetIndex() != localIndex)
			continue;

		player_info_t info;
		if (!interfaces::Engine->GetPlayerInfo(player->GetIndex(), &info))
			continue;

		int m_iObserverMode = player->m_iObserverMode();
		bool isfirstperson = m_iObserverMode == OBS_MODE_IN_EYE;

		ImGui::TextColored(isfirstperson ? ImVec4(1.0, 0.5, 0.5, 1.0) : ImVec4(1.0, 1.0, 1.0, 1.0), "%s", player->GetName().c_str());
	}

	ImGui::End();
}

static void DrawPlayerList()
{
	if (interfaces::Engine->IsTakingScreenshot())
		return;

	ImGui::SetNextWindowSizeConstraints(
        	ImVec2(150.0f, 0.0f),
        	ImVec2(FLT_MAX, FLT_MAX)
    	);

	int flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	if (!settings.menu_open)
		flags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Player List", nullptr, flags))
	{
		for (const auto& entity : EntityList::m_vecPlayers)
		{
			if (entity == nullptr)
				continue;

			std::string name = entity->GetName();

			switch (entity->m_iTeamNum())
			{
				case TEAM_BLU:
				{
					Color color = settings.colors.blu_team;
					ImVec4 textColor(color.r()/255.0f, color.g()/255.0f, color.b()/255.0f, 255);
					ImGui::TextColored(textColor, "%s", name.c_str());
					break;
				}

				case TEAM_RED:
				{
					Color color = settings.colors.red_team;
					ImVec4 textColor(color.r()/255.0f, color.g()/255.0f, color.b()/255.0f, 255);
					ImGui::TextColored(textColor, "%s", name.c_str());
					break;
				}

				default: break;
			}
		}
	}
	ImGui::End();
}

static void DrawMainWindow()
{
	if (helper::engine::IsTakingScreenshot())
		return;

	static int tab = 0;

	if (ImGui::Begin("Skill Issue", nullptr, ImGuiWindowFlags_NoCollapse))
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
				case TAB_NETVARS: DrawNetVarsTab(); break;
				default: break;
			}
			
			ImGui::EndTable();
		}
	}
	ImGui::End();
}