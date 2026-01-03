# pragma once

#include "../../../sdk/interfaces/interfaces.h"
#include "../../../sdk/helpers/helper.h"
#include "../../../settings.h"
#include "../../../sdk/handle_utils.h"
#include <vector>

struct SpectatorList
{
	void Run(CTFPlayer* pLocal)
	{
		if (!settings.misc.spectatorlist || !pLocal->IsAlive())
			return;

		int ourIndex = pLocal->GetIndex();
		int ourTeam = pLocal->m_iTeamNum();
		int count = 0;

		Color normal{255, 255, 255, 255};
		Color firstperson{255, 100, 100, 255};

		int width, height;
		helper::draw::GetScreenSize(width, height);

		int halfw;
		halfw = static_cast<int>(width * 0.5f);

		for (int i = 1; i < helper::engine::GetMaxClients(); i++)
		{
			IClientEntity* clientEnt = interfaces::EntityList->GetClientEntity(i);
			if (clientEnt == nullptr)
				continue;

			CTFPlayer* player = static_cast<CTFPlayer*>(clientEnt);
			if (!player || !player->IsPlayer() || player->IsAlive() || player == pLocal)
				continue;

			if (player->m_iTeamNum() != ourTeam)
				continue;

			CTFPlayer* m_hObserverTarget = HandleAs<CTFPlayer>(player->m_hObserverTarget());
			if (!m_hObserverTarget || m_hObserverTarget->GetIndex() != ourIndex)
				continue;

			player_info_t info;
			if (!interfaces::Engine->GetPlayerInfo(i, &info))
				continue;

			int m_iObserverMode = player->m_iObserverMode();
			bool isfirstperson = m_iObserverMode == OBS_MODE_IN_EYE;

			int textw, texth;
			helper::draw::GetTextSize(info.name, textw, texth);

			int x, y;
			x = static_cast<int>(halfw - textw*0.5f);
			y = static_cast<int>((height * 0.2f) + (18 * count));

			helper::draw::TextShadow(x, y, isfirstperson ? firstperson : normal, info.name);
			count++;
		}
	}
};