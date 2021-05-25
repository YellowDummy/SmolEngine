#include "stdafx.h"
#include "RendererPanel.h"
#include "ECS/Components/SceneStateComponent.h"

#include <Frostium3D/ImGUI/ImGuiExtension.h>

namespace SmolEngine
{
	void RendererPanel::OnUpdate(bool& open)
	{
		constexpr float padding = 170.0f;

		if (open)
		{
			ImGui::Begin("Renderer Settings", &open);
			{
				SceneStateComponent* state = WorldAdmin::GetSingleton()->GetActiveScene()->GetSceneState();
				if (state)
				{
					ImGui::SetWindowFontScale(0.8f);

					ImGui::NewLine();
					if (ImGui::CollapsingHeader("Post-processing"))
					{
						ImGui::NewLine();
						if (ImGui::Extensions::CheckBox("Bloom", state->PipelineState.m_RendererState.bBloomPass, padding, "DebugDraw", 12.0f))
							UpdateStates();

						if(ImGui::Extensions::CheckBox("Blur", state->PipelineState.m_RendererState.bBlurPass, padding, "DebugDraw", 12.0f))
							UpdateStates();

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("Graphics Pipeline"))
					{
						ImGui::NewLine();
						bool* useIBl = (bool*)&state->PipelineState.m_SceneState.UseIBL;

						if(ImGui::Extensions::CheckBox("Grid", state->PipelineState.m_RendererState.bDrawGrid, padding, "DebugDraw", 12.0f))
							UpdateStates();

						if(ImGui::Extensions::CheckBox("Skybox", state->PipelineState.m_RendererState.bDrawSkyBox, padding, "DebugDraw", 12.0f))
							UpdateStates();

						if(ImGui::Extensions::CheckBox("IBL", *useIBl, padding, "DebugDraw", 12.0f))
							UpdateStates();

						if(ImGui::Extensions::InputFloat("HDR Exposure", state->PipelineState.m_SceneState.HDRExposure, padding, "DebugDraw", 12.0f))
							UpdateStates();

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("Debugger"))
					{
						ImGui::NewLine();
						ImGui::Extensions::CheckBox("Default", m_DebugDrawState.bDefaultDraw, padding, "DebugDraw", 12.0f);
						ImGui::Extensions::CheckBox("Bullet3", m_DebugDrawState.bBullet3Draw, padding, "DebugDraw", 12.0f);
						ImGui::Extensions::CheckBox("Wireframes", m_DebugDrawState.bWireframes, padding, "DebugDraw", 12.0f);
					}
				}
			}
			ImGui::End();
		}
	}

	DebugDrawState* RendererPanel::GetDebugState()
	{
		return &m_DebugDrawState;
	}

	void RendererPanel::UpdateStates()
	{
		SceneStateComponent* state = WorldAdmin::GetSingleton()->GetActiveScene()->GetSceneState();
		Renderer::SetRenderingState(&state->PipelineState.m_RendererState);
		Renderer::SetSceneState(&state->PipelineState.m_SceneState);
	}
}