#pragma once

#include "../../sdk/definitions/keyvalues.h"
#include "../../sdk/helpers/helper.h"
#include "../../features/esp/esp.h"
#include <vector>

namespace Chams
{
	inline bool m_bMatLoaded = false;
	inline bool m_bRunning = false;
	inline IMaterial* m_mFlatMat = nullptr;
	inline std::unordered_map<int, bool> m_DrawEnts;

	inline bool Init()
	{
		if (m_mFlatMat == nullptr)
		{
			m_mFlatMat = helper::material::CreateMaterial("CoolFlatMat",
				"UnlitGeneric\n"
				"{\n"
				"	$basetexture \"white\"\n"
				"}"
			);
			m_bMatLoaded = true;
			helper::console::Print("Material vapooo created!\n");
		}

		return m_bMatLoaded;
	}

	inline bool ShouldHide(int entindex)
	{
		return m_DrawEnts.find(entindex) != m_DrawEnts.end();
	}

	inline void DrawPlayers()
	{
		m_DrawEnts.clear();

		float savedColor[3], savedBlend;
		savedBlend = interfaces::RenderView->GetBlend();

		interfaces::RenderView->GetColorModulation(savedColor);
		interfaces::RenderView->SetBlend(0.5f);
		interfaces::ModelRender->ForcedMaterialOverride(m_mFlatMat);

		for (int i = 1; i <= helper::engine::GetMaxClients(); i++)
		{
			IClientEntity* clientEntity = interfaces::EntityList->GetClientEntity(i);
			if (clientEntity == nullptr)
				continue;

			CBaseEntity* baseEntity = static_cast<CBaseEntity*>(clientEntity);
			if (baseEntity == nullptr)
				continue;

			if (!baseEntity->IsPlayer())
				continue;

			if (baseEntity->IsDormant() || !baseEntity->ShouldDraw())
				continue;

			CTFPlayer* player = static_cast<CTFPlayer*>(baseEntity);
			if (player == nullptr)
				continue;

			if (!player->IsAlive())
				continue;
			
			Color color = ESP::GetPlayerColor(baseEntity);
			float flColor[3] = {color.r()/255.0f, color.g()/255.0f, color.b()/255.0f};

			interfaces::RenderView->SetColorModulation(flColor);

			m_bRunning = true;

			m_DrawEnts[baseEntity->GetIndex()] = true;
			baseEntity->DrawModel(STUDIO_RENDER);

			m_bRunning = false;

			CBaseEntity* moveChild = player->FirstMoveChild();
			while (moveChild != nullptr)
			{
				m_bRunning = true;

				m_DrawEnts[moveChild->GetIndex()] = true;
				moveChild->DrawModel(STUDIO_RENDER);

				m_bRunning = false;

				moveChild = moveChild->NextMovePeer();
			}
		}

		interfaces::ModelRender->ForcedMaterialOverride(nullptr);
		interfaces::RenderView->SetColorModulation(savedColor);
		interfaces::RenderView->SetBlend(savedBlend);
	}

	inline void Run()
	{
		if (!settings.esp.chams)
			return;

		Init();

		m_bRunning = false;

		DrawPlayers();

		m_bRunning = false;
	}
}