#include "entitylist.h"

namespace EntityList
{
	std::vector<CBaseObject*> m_vecBuildings;
	std::vector<CTFPlayer*> m_vecPlayers;
	CTFPlayer* m_pLocalPlayer = nullptr;
	CTFPlayerResource *m_pPlayerResource = nullptr;
	std::vector<CBaseEntity*> m_vecTeammates;
	std::vector<CBaseEntity*> m_vecEnemies;
	CBaseEntity* m_pAimbotTarget = nullptr;

	void Clear()
	{
		m_vecBuildings.clear();
		m_vecPlayers.clear();
		m_vecTeammates.clear();
		m_vecEnemies.clear();
		m_pLocalPlayer = nullptr;
		m_pPlayerResource = nullptr;
	}

	// Call in FrameStageNotify -> FRAME_NET_UPDATE_END
	void Store()
	{
		Clear();

		m_pLocalPlayer = helper::engine::GetLocalPlayer();
		if (m_pLocalPlayer == nullptr)
			return;

		int localTeam = m_pLocalPlayer->m_iTeamNum();

		for (int i = 1; i < interfaces::EntityList->GetHighestEntityIndex(); i++)
		{
			IClientEntity* entity = interfaces::EntityList->GetClientEntity(i);
			if (entity == nullptr)
				continue;

			switch (entity->GetClassID())
			{
				case ETFClassID::CTFPlayer:
				{
					CTFPlayer* player = static_cast<CTFPlayer*>(entity);
					m_vecPlayers.emplace_back(player);

					if (player->m_iTeamNum() == localTeam)
						m_vecTeammates.emplace_back(player);
					else
						m_vecEnemies.emplace_back(player);
					
					break;
				}

				case ETFClassID::CObjectSentrygun:
				case ETFClassID::CObjectDispenser:
				case ETFClassID::CObjectTeleporter:
				{
					CBaseObject* building = static_cast<CBaseObject*>(entity);
					m_vecBuildings.emplace_back(building);

					CBaseEntity* builder = HandleAs<CTFPlayer*>(building->m_hBuilder());
					if (builder == nullptr)
						break;

					if (builder->m_iTeamNum() == localTeam)
						m_vecTeammates.emplace_back(static_cast<CBaseEntity*>(entity));
					else
						m_vecEnemies.emplace_back(static_cast<CBaseEntity*>(entity));

					break;
				}

				case ETFClassID::CTFPlayerResource:
				{
					m_pPlayerResource = static_cast<CTFPlayerResource*>(entity);
					break;
				}

				default: break;
			}
		}
	}

	std::vector<CTFPlayer*> GetPlayers()
	{
		return m_vecPlayers;
	}

	std::vector<CBaseObject*> GetBuildings()
	{
		return m_vecBuildings;
	}

	CTFPlayer* GetLocal()
	{
		return m_pLocalPlayer;
	}

	CTFPlayerResource* GetPlayerResources()
	{
		return m_pPlayerResource;
	}

	std::vector<CBaseEntity*> GetTeammates()
	{
		return m_vecTeammates;
	}

	std::vector<CBaseEntity*> GetEnemies()
	{
		return m_vecEnemies;
	}
}