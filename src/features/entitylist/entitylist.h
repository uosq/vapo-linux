#pragma once

#include <vector>
#include "../../sdk/helpers/helper.h"
#include "../../sdk/classes/cbaseobject.h"

namespace EntityList
{
	extern std::vector<CBaseObject*> m_vecBuildings;
	extern std::vector<CTFPlayer*> m_vecPlayers;
	extern CTFPlayer* m_pLocalPlayer;
	extern CTFPlayerResource *m_pPlayerResource;

	void Clear();
	void Store(); // Call in FrameStageNotify -> FRAME_NET_UPDATE_END
	std::vector<CTFPlayer*> GetPlayers();
	std::vector<CBaseObject*> GetBuildings();
	CTFPlayer* GetLocal();
	CTFPlayerResource* GetPlayerResources();
}