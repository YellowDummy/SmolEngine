#include "stdafx.h"
#include "MaterialLibraryInterface.h"
#include "Core/FileDialog.h"
#include "Core/FilePaths.h"

#include "ECS/Systems/JobsSystem.h"

#include "Renderer/Mesh.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Framebuffer.h"
#ifndef SMOLENGINE_OPENGL_IMPL
#include "Renderer/Vulkan/VulkanPBR.h"
#endif
#include "Renderer/EditorCamera.h"

#include <thread>
#include <imgui.h>
#include "ImGui/ImGuiExtension.h"
#include "ImGui/EditorConsole.h"

namespace SmolEngine
{
	MaterialLibraryInterface::MaterialLibraryInterface()
	{
		InitPreviewRenderer();
	}

	void MaterialLibraryInterface::Draw(bool& show)
	{
		if (show)
		{
			ImGui::Begin("Material Library", &show, ImGuiWindowFlags_MenuBar);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 10.0f });
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::MenuItem("Save as"))
					{
						const auto& result = FileDialog::SaveFile("SmolEngine Material (*.s_material)\0*.s_material\0", "new_material.s_material");
						if (result.has_value())
						{
							std::string value = result.value();
							if(MaterialLibrary::GetSinglenton()->Save(value, m_MaterialCI))
								Reset();
						}
					}

					if (ImGui::MenuItem("Load"))
					{
						Reset();
						const auto& result = FileDialog::OpenFile("SmolEngine Material (*.s_material)\0*.s_material\0");
						if (result.has_value())
						{
							std::string value = result.value();
							MaterialLibrary::GetSinglenton()->Load(value, m_MaterialCI);
							m_Buffer.albedro = m_MaterialCI.Textures[MaterialTexture::Albedro];
							m_Buffer.normal = m_MaterialCI.Textures[MaterialTexture::Normal];
							m_Buffer.metallic = m_MaterialCI.Textures[MaterialTexture::Metallic];
							m_Buffer.roughness = m_MaterialCI.Textures[MaterialTexture::Roughness];
							m_Buffer.ao = m_MaterialCI.Textures[MaterialTexture::AO];
							m_Buffer.name = m_MaterialCI.Name;
						}
					}

					if (ImGui::MenuItem("Show Preview"))
						m_bShowPreview = true;
				}
				ImGui::EndMenuBar();
				ImGui::PopStyleVar();

				ImGui::Extensions::Text("Textures", "");
				DrawTextureInfo("Albedro", m_MaterialCI.Textures[MaterialTexture::Albedro], m_Buffer.albedro);
				DrawTextureInfo("Normal", m_MaterialCI.Textures[MaterialTexture::Normal], m_Buffer.normal);
				DrawTextureInfo("Metallic", m_MaterialCI.Textures[MaterialTexture::Metallic], m_Buffer.metallic);
				DrawTextureInfo("Roughness", m_MaterialCI.Textures[MaterialTexture::Roughness], m_Buffer.roughness);
				DrawTextureInfo("AO", m_MaterialCI.Textures[MaterialTexture::AO], m_Buffer.ao);
				ImGui::NewLine();

				ImGui::Extensions::Text("Parameters", "");
				ImGui::Extensions::InputFloat("Albedro", m_MaterialCI.Albedro);
				ImGui::Extensions::InputFloat("Metallic", m_MaterialCI.Metallic);
				ImGui::Extensions::InputFloat("Roughness", m_MaterialCI.Roughness);

				ImGui::NewLine();
				if (ImGui::Button("Generate Preview", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
				{
					auto f = [&]() {

						struct RenderState
						{
							glm::ivec4 states = glm::ivec4(0); // x = albedro, y = normal, z = metallic, w = roughness
							glm::ivec4 states2 = glm::ivec4(0); // x = ao
							glm::vec4 pbrValues = glm::vec4(0); // x = metallic, y = metallic
							glm::vec4 camPos = glm::vec4(0);
						} info;

						TextureLoadedData albedroData = {};
						TextureLoadedData normalData = {};
						TextureLoadedData metallicData = {};
						TextureLoadedData roughnessData = {};
						TextureLoadedData aoData = {};

						JobsSystem::BeginSubmition();
						{
							JobsSystem::Schedule(JobPriority::General, 0, [](const std::string& path, TextureLoadedData* data)
								{
									Texture::LoadTexture(path, data);

								}, m_MaterialCI.Textures[MaterialTexture::Albedro], & albedroData);

							JobsSystem::Schedule(JobPriority::General, 0, [](const std::string& path, TextureLoadedData* data)
								{
									Texture::LoadTexture(path, data);

								}, m_MaterialCI.Textures[MaterialTexture::Normal], & normalData);

							JobsSystem::Schedule(JobPriority::General, 0, [](const std::string& path, TextureLoadedData* data)
								{
									Texture::LoadTexture(path, data);

								}, m_MaterialCI.Textures[MaterialTexture::Metallic], & metallicData);

							JobsSystem::Schedule(JobPriority::General, 0, [](const std::string& path, TextureLoadedData* data)
								{
									Texture::LoadTexture(path, data);

								}, m_MaterialCI.Textures[MaterialTexture::Roughness], & roughnessData);

							JobsSystem::Schedule(JobPriority::General, 0, [](const std::string& path, TextureLoadedData* data)
								{
									Texture::LoadTexture(path, data);

								}, m_MaterialCI.Textures[MaterialTexture::AO], & aoData);
						}
						JobsSystem::EndSubmition();

						Ref<Texture> albedro = Texture::Create(&albedroData, TextureFormat::R8G8B8A8_UNORM, false);
						Ref<Texture> normal = Texture::Create(&normalData, TextureFormat::R8G8B8A8_UNORM, false);
						Ref<Texture> metallic = Texture::Create(&metallicData, TextureFormat::R8G8B8A8_UNORM, false);
						Ref<Texture> roughness = Texture::Create(&roughnessData, TextureFormat::R8G8B8A8_UNORM, false);
						Ref<Texture> ao = Texture::Create(&aoData, TextureFormat::R8G8B8A8_UNORM, false);

#ifndef SMOLENGINE_OPENGL_IMPL
						Ref<Texture> dummy = Texture::CreateWhiteTexture();
						m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(5, dummy->GetVulkanTexture()->GetVkDescriptorImageInfo());
						m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(6, dummy->GetVulkanTexture()->GetVkDescriptorImageInfo());
						m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(7, dummy->GetVulkanTexture()->GetVkDescriptorImageInfo());
						m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(8, dummy->GetVulkanTexture()->GetVkDescriptorImageInfo());
						m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(9, dummy->GetVulkanTexture()->GetVkDescriptorImageInfo());

						if (albedro)
						{
							info.states.x = 1;
							m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(5, albedro->GetVulkanTexture()->GetVkDescriptorImageInfo());
						}
						if (normal)
						{
							info.states.y = 1;
							m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(6, normal->GetVulkanTexture()->GetVkDescriptorImageInfo());
						}
						if (metallic)
						{
							info.states.z = 1;
							m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(7, metallic->GetVulkanTexture()->GetVkDescriptorImageInfo());
						}
						if (roughness)
						{
							info.states.w = 1;
							m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(8, roughness->GetVulkanTexture()->GetVkDescriptorImageInfo());
						}
						if (ao)
						{
							info.states.x = 1;
							m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(9, ao->GetVulkanTexture()->GetVkDescriptorImageInfo());
						}
#endif

						info.pbrValues.x = m_MaterialCI.Metallic;
						info.pbrValues.y = m_MaterialCI.Roughness;
						info.camPos = glm::vec4(m_PreviewRenderingData.CameraPos, 1.0f);
						m_PreviewRenderingData.Pipeline->SubmitBuffer(100, sizeof(RenderState), &info);

						// Render
						m_PreviewRenderingData.Pipeline->BeginCommandBuffer();
						m_PreviewRenderingData.Pipeline->BeginRenderPass();
						{

							m_PreviewRenderingData.Pipeline->SubmitPushConstant(ShaderType::Vertex, sizeof(glm::mat4), &m_PreviewRenderingData.ViewProj);
							m_PreviewRenderingData.Pipeline->DrawMesh(m_PreviewRenderingData.Mesh.get());
						}
						m_PreviewRenderingData.Pipeline->EndRenderPass();
						m_PreviewRenderingData.Pipeline->EndCommandBuffer();

						m_bShowPreview = true;
					};

					std::thread worker(f);
					worker.join(); // temp
				}

				if (m_bShowPreview)
				{
					ImGui::Begin("Material Preview", &m_bShowPreview);
					ImGui::SetWindowSize({ 720, 540 });
					ImGui::Image(m_PreviewRenderingData.Framebuffer->GetImGuiTextureID(), ImVec2{ 720, 480 });
					ImGui::End();
				}
			}
			ImGui::End();
		}
	}

	void MaterialLibraryInterface::Reset()
	{
		m_Buffer = {};
		m_MaterialCI = {};
	}

	void MaterialLibraryInterface::InitPreviewRenderer()
	{
		FramebufferSpecification spec = {};
		spec.Height = 480;
		spec.Width = 720;
		spec.bResizable = false;
		spec.bTargetsSwapchain = false;
		spec.bUsedByImGui = true;
		spec.bUseMSAA = true;
		spec.NumSubpassDependencies = 0;
		spec.Attachments = { FramebufferAttachment(AttachmentFormat::Color, true) };

		m_PreviewRenderingData.Framebuffer = Framebuffer::Create(spec);
		m_PreviewRenderingData.Pipeline = std::make_shared<GraphicsPipeline>();
		m_PreviewRenderingData.Mesh = Mesh::Create(Resources + "Models/monkey.glb");

		BufferLayout mainLayout =
		{
			{ DataTypes::Float3, "aPos" },
			{ DataTypes::Float3, "aNormal" },
			{ DataTypes::Float4, "aTangent" },
			{ DataTypes::Float2, "aUV" },
			{ DataTypes::Int4,   "aBoneIDs"},
			{ DataTypes::Float4, "aWeight"}
		};
		VertexInputInfo vertexMain(sizeof(PBRVertex), mainLayout);

		GraphicsPipelineShaderCreateInfo shaderCI = {};
		{
			shaderCI.FilePaths[ShaderType::Vertex] = Resources + "Shaders/Vulkan/PBR_Preview.vert";
			shaderCI.FilePaths[ShaderType::Fragment] = Resources + "Shaders/Vulkan/PBR_Preview.frag";
		};

		GraphicsPipelineCreateInfo DynamicPipelineCI = {};
		{
			DynamicPipelineCI.VertexInputInfos = { vertexMain };
			DynamicPipelineCI.PipelineName = "PBR_PreviewPipeline";
			DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
			DynamicPipelineCI.TargetFramebuffer = m_PreviewRenderingData.Framebuffer;
		}

		assert(m_PreviewRenderingData.Pipeline->Create(&DynamicPipelineCI) == PipelineCreateResult::SUCCESS);
#ifndef SMOLENGINE_OPENGL_IMPL
		m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetIrradianceImageInfo());
		m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetBRDFLUTImageInfo());
		m_PreviewRenderingData.Pipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetPrefilteredCubeImageInfo());
#endif

		EditorCameraCreateInfo info;
		info.FOV = 45.0f;
		m_PreviewRenderingData.Camera = std::make_shared<EditorCamera>(&info);
		m_PreviewRenderingData.ViewProj = m_PreviewRenderingData.Camera->GetViewProjection();
		m_PreviewRenderingData.CameraPos = m_PreviewRenderingData.Camera->GetPosition();

	}

	void MaterialLibraryInterface::DrawTextureInfo(const char* header, std::string& outString, std::string& dummy)
	{
		std::string id = header + std::string("add");
		ImGui::PushID(id.c_str());
		if (ImGui::Extensions::InputRawString(header, dummy, header, 130.0f, true))
			outString = dummy;

		ImGui::SameLine();
		if(ImGui::SmallButton("Add"))
		{
			const auto& result = FileDialog::OpenFile("");
			if (result.has_value())
				dummy = result.value();
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("Clear"))
		{
			dummy = "";
		}
		
		outString = dummy;
		ImGui::PopID();
	}
}