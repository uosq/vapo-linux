#pragma once

#include "../../sdk/definitions/texture_group_names.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/definitions/itexture.h"
#include "../../features/esp/esp.h"
#include <unordered_set>
#include <vector>

struct GlowMaterials
{
	IMaterial* glowColor = nullptr;
	IMaterial* haloAddToScreen = nullptr;
	IMaterial* blurX = nullptr;
	IMaterial* blurY = nullptr;
	ITexture* pRtFullFrame = nullptr;
	ITexture* glowBuffer1 = nullptr;
	ITexture* glowBuffer2 = nullptr;
};

namespace Glow
{
	static GlowMaterials m_Materials;
	static std::vector<CBaseEntity*> glowEnts = {};
	static std::unordered_set<int> m_Entities = {};
	static bool m_bRunning = false;

	static void Init()
	{
		static bool initialized = false;
		if (initialized)
			return;

		m_Materials.glowColor = interfaces::MaterialSystem->FindMaterial("dev/glow_color", TEXTURE_GROUP_OTHER, true);
		
		m_Materials.haloAddToScreen = helper::material::CreateMaterial("GlowMaterial1",
			"UnlitGeneric\n"
			"{\n"
			"	$basetexture \"GlowBuffer1\"\n"
			"	$additive \"1\"\n"
			"}"
		);

		m_Materials.blurX = helper::material::CreateMaterial("GlowMatBlurX",
			"BlurFilterX\n"
			"{\n"
			"	$basetexture \"GlowBuffer1\"\n"
			"}"
		);

		m_Materials.blurY = helper::material::CreateMaterial("GlowMatBlurY",
			"BlurFilterY\n"
			"{\n"
			"	$basetexture \"GlowBuffer2\"\n"
			"}"
		);

		m_Materials.pRtFullFrame = interfaces::MaterialSystem->FindTexture("_rt_FullFrameFB", "RenderTargets", true);

		m_Materials.glowBuffer1 = interfaces::MaterialSystem->CreateNamedRenderTargetTextureEx(
			"GlowBuffer1",
			m_Materials.pRtFullFrame->GetActualWidth(),
			m_Materials.pRtFullFrame->GetActualHeight(),
			RT_SIZE_LITERAL,
			IMAGE_FORMAT_RGB888,
			MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA,
			CREATERENDERTARGETFLAGS_HDR
		);
	
		m_Materials.glowBuffer2 = interfaces::MaterialSystem->CreateNamedRenderTargetTextureEx(
			"GlowBuffer2",
			m_Materials.pRtFullFrame->GetActualWidth(),
			m_Materials.pRtFullFrame->GetActualHeight(),
			RT_SIZE_LITERAL,
			IMAGE_FORMAT_RGB888,
			MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA,
			CREATERENDERTARGETFLAGS_HDR
		);

		initialized = true;
	}

	static bool ShouldHide(int entindex)
	{
		return m_Entities.find(entindex) != m_Entities.end();
	}

	static void DrawEntities()
	{
		for (auto ent : glowEnts)
		{
			Color color = ent->IsPlayer() ? ESP::GetPlayerColor(ent) : ESP::GetBuildingColor(reinterpret_cast<CBaseObject*>(ent));
			float mod[3] = {color.r()/255.0f, color.g()/255.0f, color.b()/255.0f};
			interfaces::RenderView->SetColorModulation(mod);
			m_bRunning = true;
			ent->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);

			auto moveChild = ent->FirstShadowChild();
			int passes = 0;
			while (moveChild != nullptr && passes <= 32 && moveChild->ShouldDraw())
			{
				if (settings.esp.weapon && static_cast<CBaseEntity*>(moveChild)->IsWeapon())
				{
					color = settings.colors.weapon;
					// this is fucking stupid
					// why is a array not assignable?
					mod[0] = color.r()/255.0f;
					mod[1] = color.g()/255.0f;
					mod[2] = color.b()/255.0f;
					interfaces::RenderView->SetColorModulation(mod);
				}

				moveChild->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);
				moveChild = moveChild->NextShadowPeer();
				passes++;
			}
			m_bRunning = false;
		}
	}

	static void GetEntities()
	{
		for (auto entity : EntityList::m_vecPlayers)
		{
			if (entity->IsDormant() || !entity->ShouldDraw())
				continue;

			if (!entity->IsPlayer())
				continue;

			glowEnts.emplace_back(entity);
		}

		for (auto entity : EntityList::m_vecBuildings)
		{
			if (entity->IsDormant() || !entity->ShouldDraw())
				continue;

			if (entity->m_iHealth() <= 0)
				continue;

			glowEnts.emplace_back(reinterpret_cast<CBaseEntity*>(entity));
		}
	}

	// Call in DoPostScreenSpaceEffects
	static void Run()
	{
		m_bRunning = false;
		if (!m_Entities.empty())
			m_Entities.clear();
		if (!glowEnts.empty())
			glowEnts.clear();

		if (settings.esp.blur == 0 && settings.esp.stencil == 0)
			return;

		Init();
		
		auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
		if (!pRenderContext)
			return;

		GetEntities();
		
		if (glowEnts.empty())
			return;

		int w, h;
		interfaces::Engine->GetScreenSize(w, h);

		{	// stencil pass
			pRenderContext->SetStencilEnable(true);
			float savedBlend = interfaces::RenderView->GetBlend();
			interfaces::ModelRender->ForcedMaterialOverride(m_Materials.glowColor);
			interfaces::RenderView->SetBlend(0);

			pRenderContext->SetStencilReferenceValue(1);
			pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_ALWAYS);
			pRenderContext->SetStencilPassOperation(STENCILOPERATION_REPLACE);
			pRenderContext->SetStencilFailOperation(STENCILOPERATION_KEEP);
			pRenderContext->SetStencilZFailOperation(STENCILOPERATION_REPLACE);

			DrawEntities();

			interfaces::ModelRender->ForcedMaterialOverride(nullptr);
			interfaces::RenderView->SetBlend(savedBlend);
			pRenderContext->SetStencilEnable(false);
		}

		{	// color pass
			pRenderContext->PushRenderTargetAndViewport();

			float savedColor[3], savedBlend;
			interfaces::RenderView->GetColorModulation(savedColor);
			savedBlend = interfaces::RenderView->GetBlend();

			interfaces::RenderView->SetBlend(1.0f);
			pRenderContext->SetRenderTarget(m_Materials.glowBuffer1);
			pRenderContext->Viewport(0, 0, w, h);
			pRenderContext->ClearColor3ub(0, 0, 0);
			pRenderContext->ClearBuffers(true, false, false);
			interfaces::ModelRender->ForcedMaterialOverride(m_Materials.glowColor);

			DrawEntities();

			interfaces::ModelRender->ForcedMaterialOverride(nullptr);
			interfaces::RenderView->SetBlend(savedBlend);
			interfaces::RenderView->SetColorModulation(savedColor);

			pRenderContext->PopRenderTargetAndViewport();
		}

		// blur pass
		if (settings.esp.blur > 0)
		{
			pRenderContext->PushRenderTargetAndViewport();
			pRenderContext->Viewport(0, 0, w, h);

			for (int i = 0; i < settings.esp.blur; ++i)
			{
				pRenderContext->SetRenderTarget(m_Materials.glowBuffer2);
				pRenderContext->DrawScreenSpaceRectangle(m_Materials.blurX, 0, 0, w, h, 0, 0, w - 1, h - 1, w, h);
				pRenderContext->SetRenderTarget(m_Materials.glowBuffer1);
				pRenderContext->DrawScreenSpaceRectangle(m_Materials.blurY, 0, 0, w, h, 0, 0, w - 1, h - 1, w, h);
			}

			pRenderContext->PopRenderTargetAndViewport();
		}

		{	// draw pass
			pRenderContext->SetStencilEnable(true);
			pRenderContext->SetStencilWriteMask(0);
			pRenderContext->SetStencilTestMask(0xFF);

			pRenderContext->SetStencilReferenceValue(1);
			pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_NOTEQUAL);

			pRenderContext->SetStencilPassOperation(STENCILOPERATION_KEEP);
			pRenderContext->SetStencilFailOperation(STENCILOPERATION_KEEP);
			pRenderContext->SetStencilZFailOperation(STENCILOPERATION_KEEP);

			// this is from amalgam
			if (settings.esp.stencil)
			{
				int side = (settings.esp.stencil + 1) / 2;
				pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, -side, 0, w, h, 0, 0, w - 1, h - 1, w, h);
				pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, 0, -side, w, h, 0, 0, w - 1, h - 1, w, h);
				pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, side, 0, w, h, 0, 0, w - 1, h - 1, w, h);
				pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, 0, side, w, h, 0, 0, w - 1, h - 1, w, h);
				
				int corner = settings.esp.stencil / 2;
				if (corner)
				{
					pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, -corner, -corner, w, h, 0, 0, w - 1, h - 1, w, h);
					pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, corner, corner, w, h, 0, 0, w - 1, h - 1, w, h);
					pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, corner, -corner, w, h, 0, 0, w - 1, h - 1, w, h);
					pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, -corner, corner, w, h, 0, 0, w - 1, h - 1, w, h);
				}
			}

			if (settings.esp.blur)
				pRenderContext->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, 0, 0, w, h, 0, 0, w - 1, h - 1, w, h);

			pRenderContext->SetStencilEnable(false);
		}
	}
}