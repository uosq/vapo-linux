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
	inline std::vector<int> drawEnts;

	inline bool Init()
	{
		if (m_mFlatMat == nullptr)
		{
			// Im probably corrupting the heap doing this
			// But fuck you Valve! KeyValuesSystem crashes my game
			KeyValues* kv = new KeyValues("UnlitGeneric");
			kv->LoadFromBuffer(
				"vapoo",
				"UnlitGeneric\n"
				"{\n"
				"	$basetexture \"white\"\n"
				"}"
			);

			m_mFlatMat = interfaces::MaterialSystem->CreateMaterial("vapoo", kv);
			m_bMatLoaded = true;
			helper::console::Print("Material vapooo created!\n");
		}

		return m_bMatLoaded;
	}

	inline bool ShouldHide(int entindex)
	{
		for (auto& index : drawEnts)
		{
			if (entindex == index)
				return true;
		}

		return false;
	}

	inline void DrawPlayers()
	{
		drawEnts.clear();

		float savedColor[3], savedBlend;
		interfaces::RenderView->GetColorModulation(savedColor);
		savedBlend = interfaces::RenderView->GetBlend();

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

			if (baseEntity->IsDormant())
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
			drawEnts.emplace_back(baseEntity->GetIndex());
			baseEntity->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);
			m_bRunning = false;
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