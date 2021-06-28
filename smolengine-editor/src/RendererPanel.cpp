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
		constexpr float padding = 140.0f;

		if (open)
		{
			ImGui::Begin("Pipeline Settings", &open);
			{
				SceneStateComponent* scene_state = WorldAdmin::GetSingleton()->GetActiveScene()->GetSceneState();
				ScenePipelineState& pipline_state = scene_state->PipelineState;
				RendererState& render_state = pipline_state.State;
				EnvironmentData& environment = pipline_state.Environment;

				if (scene_state)
				{
					ImGui::SetWindowFontScale(0.8f);

					ImGui::NewLine();
					if (ImGui::CollapsingHeader("Environment"))
					{
						ImGui::NewLine();

						ImGui::Extensions::Combo("Type", "Dynamic\0Static\0", pipline_state.EnvironmentFlags, padding, "IBL", 12.0f);
						if (scene_state->PipelineState.EnvironmentFlags == 0)
						{
							ImGui::Separator();
							ImGui::NewLine();

							glm::vec3* sunPos = (glm::vec3*)(&environment.SkyProperties.SunPosition);
							glm::vec3* rayOrigin = (glm::vec3*)(&environment.SkyProperties.RayOrigin);

							ImGui::Extensions::DragFloat3Base("Sun Position", *sunPos, padding, "IBL", 12.0f);
							ImGui::Extensions::DragFloat3Base("Ray Origin", *rayOrigin, padding, "IBL", 12.0f);
							ImGui::Extensions::InputFloat("Sun Intensity", environment.SkyProperties.SunIntensity, padding, "IBL", 12.0f);
							ImGui::Extensions::InputFloat("Planet Radius", environment.SkyProperties.PlanetRadius, padding, "IBL", 12.0f);
							ImGui::Extensions::InputFloat("Atm Radius", environment.SkyProperties.AtmosphereRadius, padding, "IBL", 12.0f);
							ImGui::Extensions::InputFloat("Mie Direction", environment.SkyProperties.MieScatteringDirection, padding, "IBL", 12.0f);


							int* numCirrus = (int*)(&environment.SkyProperties.NumCirrusCloudsIterations);
							int* numCumulus = (int*)(&environment.SkyProperties.NumCumulusCloudsIterations);
							ImGui::Extensions::InputInt("Cirrus Clouds", *numCirrus, padding, "IBL", 12.0f);
							ImGui::Extensions::InputInt("Cumulus Clouds", *numCumulus, padding, "IBL", 12.0f);

							ImGui::Extensions::CheckBox("Update PBR Maps", environment.bGeneratePBRMaps, padding, "IBL", 12.0f);

							ImGui::SetCursorPosX(12.0f);
							if (ImGui::Button("Update"))
							{
								DeferredRenderer::SetDynamicSkyboxProperties(environment.SkyProperties, environment.bGeneratePBRMaps);
							}

							ImGui::SameLine();
							if (ImGui::Button("Reset"))
							{
								environment.SkyProperties = {};
								DeferredRenderer::SetDynamicSkyboxProperties(environment.SkyProperties, true);
							}

							ImGui::NewLine();
							ImGui::Separator();
							ImGui::NewLine();
						}
						else
						{
							ImGui::Separator();
							ImGui::NewLine();

							ImGui::Extensions::Combo("Format", "R8G8B8A8\0R16G16B16A16\0", pipline_state.FormatFlags, padding, "IBL", 12.0f);
							ImGui::NewLine();
							ImGui::Image(TexturesLoader::Get()->m_BackgroundIcon.GetImGuiTexture(), ImVec2{ 60, 60 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
								{
									std::string& path = *(std::string*)payload->Data;
									if (EditorLayer::FileExtensionCheck(path, ".ktx"))
									{
										TextureFormat format = pipline_state.FormatFlags == 0 ? TextureFormat::R8G8B8A8_UNORM : TextureFormat::R16G16B16A16_SFLOAT;
										environment.CubeMap = new CubeMap();
										CubeMap::Create(environment.CubeMap, path, format);
										DeferredRenderer::SetEnvironmentCube(environment.CubeMap);
									}
								}
								ImGui::EndDragDropTarget();
							}
							ImGui::SameLine();
							ImGui::TextUnformatted("Note: only .ktx textures are supported.");

							ImGui::NewLine();
							ImGui::Separator();
							ImGui::NewLine();
						}

						bool* value = (bool*)(&render_state.Lighting.UseIBL);
						if (ImGui::Extensions::CheckBox("Enabled", *value, padding, "IBL", 12.0f))
						{
							render_state.bDrawSkyBox = *value;
							UpdateStates();
						}

						if (ImGui::Extensions::CheckBox("Show", render_state.bDrawSkyBox, padding, "IBL", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Strength", render_state.Lighting.IBLStrength, padding, "IBL", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::ColorInput4("Ambient Color", render_state.Lighting.AmbientColor, padding, "IBL", 12.0f))
							UpdateStates();

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("Bloom Settings"))
					{
						ImGui::NewLine();
						if (ImGui::Extensions::CheckBox("Enabled", render_state.bBloom, padding, "Bloom", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Exposure", render_state.Bloom.Exposure, padding, "Bloom", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Scale", render_state.Bloom.Scale, padding, "Bloom", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Strength", render_state.Bloom.Strength, padding, "Bloom", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Threshold", render_state.Bloom.Threshold, padding, "Bloom", 12.0f))
							UpdateStates();

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("FXAA Settings"))
					{
						ImGui::NewLine();
						if (ImGui::Extensions::CheckBox("Enabled", render_state.bFXAA, padding, "FXAA", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Threshold Max", render_state.FXAA.EdgeThresholdMax, padding, "FXAA", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Threshold Min", render_state.FXAA.EdgeThresholdMin, padding, "FXAA", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("Iterations", render_state.FXAA.Iterations, padding, "FXAA", 12.0f))
							UpdateStates();

						if (ImGui::Extensions::InputFloat("SubPixelQuality", render_state.FXAA.SubPixelQuality, padding, "FXAA", 12.0f))
							UpdateStates();

						ImGui::NewLine();
					}

					if (ImGui::CollapsingHeader("Volumes"))
					{
						ImGui::NewLine();
						ImGui::SetCursorPosX(12.0f);
						ImGui::TextUnformatted("Dirt Mask");
						auto& dirt = scene_state->PipelineState.DirtMask;
						if (dirt.bReset)
						{
							dirt = {};
							DeferredRenderer::SetDirtMask(nullptr, 1.0f, 0.1f);
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
									DeferredRenderer::SetDirtMask(dirt.Texture.get(), dirt.Intensity, dirt.IntensityBase);
								}
							}
							ImGui::EndDragDropTarget();
						}

						ImGui::SameLine();
						if (ImGui::Button("Reset")) { dirt.bReset = true; }
						ImGui::NewLine();

						if (ImGui::Extensions::InputFloat("Dirt BaseIntensity", dirt.IntensityBase, padding, "Dirt", 12.0f))
						{
							DeferredRenderer::SetDirtMask(dirt.Texture.get(), dirt.Intensity, dirt.IntensityBase);
						}

						if (ImGui::Extensions::InputFloat("Dirt Intensity", dirt.Intensity, padding, "Dirt", 12.0f))
						{
							DeferredRenderer::SetDirtMask(dirt.Texture.get(), dirt.Intensity, dirt.IntensityBase);
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
						if (ImGui::Extensions::CheckBox("Grid", render_state.bDrawGrid, padding, "DebugDraw", 12.0f))
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