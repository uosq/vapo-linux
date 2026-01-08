#pragma once

#include "../../sdk/definitions/texture_group_names.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/definitions/itexture.h"
#include "../../features/esp/esp.h"
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

	static void Init()
	{
		static bool initialized = false;
		if (initialized)
			return;

		glowEnts = {};

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

	static void DrawEntities()
	{
		for (const auto& ent : glowEnts)
		{
			Color color = ESP::GetPlayerColor(ent);
			float mod[3] = {color.r()/255.0f, color.g()/255.0f, color.b()/255.0f};
			interfaces::RenderView->SetColorModulation(mod);

			ent->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);
		}
	}

	static void GetEntities()
	{
		int maxclients = interfaces::Engine->GetMaxClients();
		for (int i = 1; i <= maxclients; i++)
		{
			IClientEntity* clientEntity = interfaces::EntityList->GetClientEntity(i);
			if (clientEntity == nullptr)
				continue;

			CBaseEntity* baseEntity = static_cast<CBaseEntity*>(clientEntity);
			if (baseEntity == nullptr)
				continue;

			if (baseEntity->IsDormant() || !baseEntity->ShouldDraw())
				continue;

			if (!baseEntity->IsPlayer() && !baseEntity->IsBuilding())
				continue;

			glowEnts.emplace_back(baseEntity);

			/* gotta fix movechild first
			auto movechild = baseEntity->FirstMoveChild();
			while (movechild != nullptr)
			{
				glowEnts.emplace_back(movechild);
				movechild->NextMovePeer();
			}
			*/
		}
	}

	// Call in DoPostScreenSpaceEffects
	static void Run()
	{
		if (settings.esp.blur == 0 && settings.esp.stencil == 0)
			return;

		Init();
		
		if (!glowEnts.empty())
			glowEnts.clear();
		
		auto render = interfaces::MaterialSystem->GetRenderContext();
		if (!render)
			return;

		GetEntities();
		
		if (glowEnts.empty())
			return;

		int w, h;
		interfaces::Engine->GetScreenSize(w, h);

		{	// stencil pass
			render->SetStencilEnable(true);
			float savedBlend = interfaces::RenderView->GetBlend();
			interfaces::ModelRender->ForcedMaterialOverride(m_Materials.glowColor);
			interfaces::RenderView->SetBlend(0);

			render->SetStencilReferenceValue(1);
			render->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_ALWAYS);
			render->SetStencilPassOperation(STENCILOPERATION_REPLACE);
			render->SetStencilFailOperation(STENCILOPERATION_KEEP);
			render->SetStencilZFailOperation(STENCILOPERATION_REPLACE);

			DrawEntities();

			interfaces::ModelRender->ForcedMaterialOverride(nullptr);
			interfaces::RenderView->SetBlend(savedBlend);
			render->SetStencilEnable(false);
		}

		{	// color pass
			render->PushRenderTargetAndViewport();

			float savedColor[3], savedBlend;
			interfaces::RenderView->GetColorModulation(savedColor);
			savedBlend = interfaces::RenderView->GetBlend();

			interfaces::RenderView->SetBlend(1.0f);
			render->SetRenderTarget(m_Materials.glowBuffer1);
			render->Viewport(0, 0, w, h);
			render->ClearColor3ub(0, 0, 0);
			render->ClearBuffers(true, false, false);
			interfaces::ModelRender->ForcedMaterialOverride(m_Materials.glowColor);

			DrawEntities();

			interfaces::ModelRender->ForcedMaterialOverride(nullptr);
			interfaces::RenderView->SetBlend(savedBlend);
			interfaces::RenderView->SetColorModulation(savedColor);

			render->PopRenderTargetAndViewport();
		}

		if (settings.esp.blur)
		{
			render->PushRenderTargetAndViewport();
			render->Viewport(0, 0, w, h);

			for (int i = 0; i < settings.esp.blur; ++i)
			{
				render->SetRenderTarget(m_Materials.glowBuffer2);
				render->DrawScreenSpaceRectangle(m_Materials.blurX, 0, 0, w, h, 0, 0, w - 1, h - 1, w, h);
				render->SetRenderTarget(m_Materials.glowBuffer1);
				render->DrawScreenSpaceRectangle(m_Materials.blurY, 0, 0, w, h, 0, 0, w - 1, h - 1, w, h);
			}

			render->PopRenderTargetAndViewport();
		}

		{
			render->SetStencilEnable(true);
			render->SetStencilWriteMask(0);
			render->SetStencilTestMask(0xFF);

			render->SetStencilReferenceValue(1);
			render->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_NOTEQUAL);

			render->SetStencilPassOperation(STENCILOPERATION_KEEP);
			render->SetStencilFailOperation(STENCILOPERATION_KEEP);
			render->SetStencilZFailOperation(STENCILOPERATION_KEEP);

			if (settings.esp.stencil)
			{
				int side = (settings.esp.stencil + 1) / 2;
				render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, -side, 0, w, h, 0, 0, w - 1, h - 1, w, h);
				render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, 0, -side, w, h, 0, 0, w - 1, h - 1, w, h);
				render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, side, 0, w, h, 0, 0, w - 1, h - 1, w, h);
				render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, 0, side, w, h, 0, 0, w - 1, h - 1, w, h);
				
				int corner = settings.esp.stencil / 2;
				if (corner)
				{
					render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, -corner, -corner, w, h, 0, 0, w - 1, h - 1, w, h);
					render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, corner, corner, w, h, 0, 0, w - 1, h - 1, w, h);
					render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, corner, -corner, w, h, 0, 0, w - 1, h - 1, w, h);
					render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, -corner, corner, w, h, 0, 0, w - 1, h - 1, w, h);
				}
			}

			if (settings.esp.blur)
				render->DrawScreenSpaceRectangle(m_Materials.haloAddToScreen, 0, 0, w, h, 0, 0, w - 1, h - 1, w, h);

			render->SetStencilEnable(false);
		}
	}
}