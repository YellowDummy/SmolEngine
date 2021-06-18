#include "stdafx.h"
#include "RendererPanel.h"
#include "ECS/Components/SceneStateComponent.h"

#include <Frostium3D/ImGUI/ImGuiExtension.h>

namespace SmolEngine
{
	void RendererPanel::SetActiveDebugDraw(bool val)
	{
		m_DebugDrawState.bDefaultDraw = val;
		SceneStateComponent* state = WorldAdmin::GetSingleton()->GetActiveScene()->GetSceneState();
		DeferredRenderer::SetRendererState(&state->PipelineState.State);
	}

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

						if (ImGui::Extensions::CheckBox("FXAA", state->PipelineState.State.bFXAA, padding, "DebugDraw", 12.0f))
						{
							UpdateStates();
						}

						if (ImGui::Extensions::CheckBox("HDR", state->PipelineState.State.bHDR, padding, "DebugDraw", 12.0f))
						{
							UpdateStates();
						}

						if (ImGui::Extensions::CheckBox("SSAO", state->PipelineState.State.bSSAO, padding, "DebugDraw", 12.0f))
						{
							UpdateStates();
						}

						if (ImGui::Extensions::Combo("PP", "Bloom\0Blur\0", state->PipelineState.PostProcessingFlag, padding, "DebugDraw", 12.0f))
						{
							state->PipelineState.State.ePostProcessing = (PostProcessingFlags)state->PipelineState.PostProcessingFlag;
							UpdateStates();
						}

						if (ImGui::Extensions::Combo("DebugView", "None\0Albedro\0Position\0Normals\0Materials\0Emission\0ShadowMap\0ShadowMapCood\0AO\0", 
							state->PipelineState.DebugViewFlag, padding, "DebugDraw", 12.0f))
						{
							state->PipelineState.State.eDebugView = (DebugViewFlags)state->PipelineState.DebugViewFlag;
							UpdateStates();
						}

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("Graphics Pipeline"))
					{
						ImGui::NewLine();
						bool* useIBl = (bool*)&state->PipelineState.State.SceneState.UseIBL;

						if(ImGui::Extensions::CheckBox("Grid", state->PipelineState.State.bDrawGrid, padding, "DebugDraw", 12.0f))
							UpdateStates();

						if(ImGui::Extensions::CheckBox("Skybox", state->PipelineState.State.bDrawSkyBox, padding, "DebugDraw", 12.0f))
							UpdateStates();

						if(ImGui::Extensions::CheckBox("IBL", *useIBl, padding, "DebugDraw", 12.0f))
							UpdateStates();

						if(ImGui::Extensions::InputFloat("HDR Exposure", state->PipelineState.State.SceneState.HDRExposure, padding, "DebugDraw", 12.0f))
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
		DeferredRenderer::SetRendererState(&state->PipelineState.State);
	}
}