#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../settings.h"
#include "../sdk/helpers/helper.h"

enum TabMenu
{
	TAB_AIMBOT = 0,
	TAB_ESP,
	TAB_MISC
};

static void DrawTabButtons(int &tab)
{
	ImGui::BeginGroup();

	if (ImGui::Button("Aimbot", ImVec2(-1, 0)))
		tab = TAB_AIMBOT;

	if (ImGui::Button("ESP", ImVec2(-1, 0)))
		tab = TAB_ESP;

	if (ImGui::Button("Misc", ImVec2(-1, 0)))
		tab = TAB_MISC;

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

	ImGui::Begin("Spectator List", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	int maxclients = helper::engine::GetMaxClients();
	if (!helper::engine::IsInMatch() || maxclients <= 1)
		return ImGui::End();

	CTFPlayer* pLocal = helper::engine::GetLocalPlayer();
	if (pLocal == nullptr || !pLocal->IsAlive())
		return ImGui::End();

	int localTeam = pLocal->m_iTeamNum();
	int localIndex = pLocal->GetIndex();

	for (int i = 1; i <= maxclients; i++)
	{
		IClientEntity* clientEnt = interfaces::EntityList->GetClientEntity(i);
		if (clientEnt == nullptr)
			continue;

		CTFPlayer* player = static_cast<CTFPlayer*>(clientEnt);
		if (!player || !player->IsPlayer() || player->IsAlive() || player == pLocal)
			continue;

		if (player->m_iTeamNum() != localTeam)
			continue;

		CTFPlayer* m_hObserverTarget = HandleAs<CTFPlayer>(player->m_hObserverTarget());
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

	if (ImGui::Begin("Vapo Linux", nullptr, 0))
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
				default: break;
			}
			
			ImGui::EndTable();
		}
	}
	ImGui::End();
}