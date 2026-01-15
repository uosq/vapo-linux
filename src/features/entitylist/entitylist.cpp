#include "entitylist.h"

namespace EntityList
{
	std::vector<CBaseObject*> m_vecBuildings;
	std::vector<CTFPlayer*> m_vecPlayers;
	CTFPlayer* m_pLocalPlayer = nullptr;
	CTFPlayerResource *m_pPlayerResource = nullptr;

	void Clear()
	{
		m_vecBuildings.clear();
		m_vecPlayers.clear();
		m_pLocalPlayer = nullptr;
		m_pPlayerResource = nullptr;
	}

	// Call in FrameStageNotify -> FRAME_NET_UPDATE_END
	void Store()
	{
		Clear();

		m_pLocalPlayer = helper::engine::GetLocalPlayer();

		for (int i = 1; i < interfaces::EntityList->GetHighestEntityIndex(); i++)
		{
			IClientEntity* entity = interfaces::EntityList->GetClientEntity(i);
			if (entity == nullptr)
				continue;

			switch (entity->GetClassID())
			{
				case ETFClassID::CTFPlayer:
				{
					m_vecPlayers.emplace_back(static_cast<CTFPlayer*>(entity));
					break;
				}

				case ETFClassID::CObjectSentrygun:
				case ETFClassID::CObjectDispenser:
				case ETFClassID::CObjectTeleporter:
				{
					m_vecBuildings.emplace_back(static_cast<CBaseObject*>(entity));
					break;
				}

				case ETFClassID::CTFPlayerResource:
				{
					m_pPlayerResource = reinterpret_cast<CTFPlayerResource*>(entity);
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
}