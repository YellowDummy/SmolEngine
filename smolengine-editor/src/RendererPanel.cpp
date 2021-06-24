#include "stdafx.h"
#include "RendererPanel.h"
#include "TexturesLoader.h"
#include "EditorLayer.h"
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
			ImGui::Begin("Pipeline Settings", &open);
			{
				SceneStateComponent* scene_state = WorldAdmin::GetSingleton()->GetActiveScene()->GetSceneState();
				RendererState& state = scene_state->PipelineState.State;

				if (scene_state)
				{
					ImGui::SetWindowFontScale(0.8f);

					ImGui::NewLine();
					if (ImGui::CollapsingHeader("Image-Based Lighting"))
					{
						ImGui::NewLine();
						bool* value = (bool*)(&state.Lighting.UseIBL);
						if (ImGui::Extensions::CheckBox("Enabled", *value, padding, "IBL", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::CheckBox("Draw Skybox", state.bDrawSkyBox, padding, "IBL", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Strength", state.Lighting.IBLStrength, padding, "IBL", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::ColorInput4("Ambient Color", state.Lighting.AmbientColor, padding, "IBL", 12.0f))
							UpdateStates();

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("Bloom Settings"))
					{
						ImGui::NewLine();
						if (ImGui::Extensions::CheckBox("Enabled", state.bBloom, padding, "Bloom", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Exposure", state.Bloom.Exposure, padding, "Bloom", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Scale", state.Bloom.Scale, padding, "Bloom", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Strength", state.Bloom.Strength, padding, "Bloom", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Threshold", state.Bloom.Threshold, padding, "Bloom", 12.0f))
							UpdateStates();

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("FXAA Settings"))
					{
						ImGui::NewLine();
						if (ImGui::Extensions::InputFloat("Threshold Max", state.FXAA.EdgeThresholdMax, padding, "FXAA", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Threshold Min", state.FXAA.EdgeThresholdMin, padding, "FXAA", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Iterations", state.FXAA.Iterations, padding, "FXAA", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("SubPixelQuality", state.FXAA.SubPixelQuality, padding, "FXAA", 12.0f))
							UpdateStates();

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("Post Processing Volumes"))
					{
						ImGui::NewLine();
						ImGui::SetCursorPosX(12.0f);
						ImGui::TextUnformatted("Dirt Mask");
						auto& dirt = scene_state->PipelineState.DirtMask;
						if (dirt.bReset)
						{
							dirt = {};
							DeferredRenderer::SetDirtMask(nullptr, 1.0f);
						}

						Texture* tex = dirt.Texture != nullptr ? dirt.Texture.get() : &TexturesLoader::Get()->m_BackgroundIcon;
						ImGui::NewLine();
						ImGui::SetCursorPosX(22.0f);
						ImGui::Image(tex->GetImGuiTexture(), ImVec2{ 60, 60 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
							{
								std::string& path = *(std::string*)payload->Data;
								if (EditorLayer::FileExtensionCheck(path, ".png") || EditorLayer::FileExtensionCheck(path, ".jpg"))
								{
									dirt.Path = path;
									dirt.Texture = std::make_shared<Texture>();
									Texture::Create(path, dirt.Texture.get(), TextureFormat::R8G8B8A8_UNORM, true, true);
									DeferredRenderer::SetDirtMask(dirt.Texture.get(), dirt.Intensity);
								}
							}
							ImGui::EndDragDropTarget();
						}

						ImGui::SameLine();
						if (ImGui::Button("Reset")) { dirt.bReset = true; }
						ImGui::NewLine();
						if (ImGui::Extensions::InputFloat("Dirt Intensity", dirt.Intensity, padding, "Dirt", 12.0f))
						{
							DeferredRenderer::SetDirtMask(dirt.Texture.get(), dirt.Intensity);
						}
						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("Debug Settings"))
					{
						ImGui::NewLine();

						if (ImGui::Extensions::Combo("View", "None\0Albedro\0Position\0Normals\0Materials\0Emission\0ShadowMap\0ShadowMapCood\0AO\0",
							scene_state->PipelineState.DebugViewFlag, padding, "DebugDraw", 12.0f))
						{
							scene_state->PipelineState.State.eDebugView = (DebugViewFlags)scene_state->PipelineState.DebugViewFlag;
							UpdateStates();
						}

						ImGui::Extensions::CheckBox("Default Draw", m_DebugDrawState.bDefaultDraw, padding, "DebugDraw", 12.0f);
						ImGui::Extensions::CheckBox("Bullet3 Draw", m_DebugDrawState.bBullet3Draw, padding, "DebugDraw", 12.0f);
						ImGui::Extensions::CheckBox("Wireframes", m_DebugDrawState.bWireframes, padding, "DebugDraw", 12.0f);
						if (ImGui::Extensions::CheckBox("Grid", state.bDrawGrid, padding, "DebugDraw", 12.0f))
							UpdateStates();

						ImGui::NewLine();
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