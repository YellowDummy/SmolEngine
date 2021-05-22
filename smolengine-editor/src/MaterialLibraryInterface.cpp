#include "stdafx.h"
#include "MaterialLibraryInterface.h"
#include "EditorLayer.h"

#include "ECS/Systems/JobsSystem.h"

#include <imgui/imgui.h>
#include <Frostium3D/ImGUI/ImGuiExtension.h>
#include <Frostium3D/Utils/Utils.h>
#include <Frostium3D/GraphicsPipeline.h>
#include <Frostium3D/GraphicsContext.h>
#include <Frostium3D/Vulkan/VulkanPBR.h>

namespace SmolEngine
{
	MaterialLibraryInterface::MaterialLibraryInterface(EditorLayer* editor)
	{
		InitPreviewRenderer();

		m_RemoveTexture = &editor->m_RemoveButton;
		m_FolderTexture = &editor->m_FolderButton;
	}

	MaterialLibraryInterface::~MaterialLibraryInterface()
	{

	}

	void MaterialLibraryInterface::Update(bool& show)
	{
		if (show)
		{
			ImGui::Begin("Material Library", &show, ImGuiWindowFlags_MenuBar);
			{
				// Menu Bar
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 10.0f });
					if (ImGui::BeginMenuBar())
					{
						if (ImGui::MenuItem("Save as"))
						{
							const auto& result = Utils::SaveFile("SmolEngine Material (*.s_material)\0*.s_material\0", "new_material.s_material");
							if (result.has_value())
								MaterialLibrary::GetSinglenton()->Save(result.value(), m_MaterialCI);
						}

						if (ImGui::MenuItem("Load"))
						{
							Reset();
							const auto& result = Utils::OpenFile("SmolEngine Material (*.s_material)\0*.s_material\0");
							if (result.has_value())
							{
								std::string value = result.value();
								MaterialLibrary::GetSinglenton()->Load(value, m_MaterialCI);
								auto& textures = m_MaterialCI.GetTexturesInfo();

								m_Buffer.albedro = textures[MaterialTexture::Albedro];
								m_Buffer.normal = textures[MaterialTexture::Normal];
								m_Buffer.metallic = textures[MaterialTexture::Metallic];
								m_Buffer.roughness = textures[MaterialTexture::Roughness];
								m_Buffer.ao = textures[MaterialTexture::AO];
							}
						}
					}
					ImGui::EndMenuBar();
					ImGui::PopStyleVar();
				}

				auto& textures = m_MaterialCI.GetTexturesInfo();
				DrawTextureInfo("Albedro", textures[MaterialTexture::Albedro], m_Buffer.albedro);
				DrawTextureInfo("Normal", textures[MaterialTexture::Normal], m_Buffer.normal);
				DrawTextureInfo("Metallic", textures[MaterialTexture::Metallic], m_Buffer.metallic);
				DrawTextureInfo("Roughness", textures[MaterialTexture::Roughness], m_Buffer.roughness);
				DrawTextureInfo("AO", textures[MaterialTexture::AO], m_Buffer.ao);

				ImGui::NewLine();
				if (ImGui::Button("Generate Preview", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
					RenderImage();

				ImGui::Begin("Material Preview");
				ImGui::SetWindowSize({ 720, 540 });
				ImGui::Image(m_Data.Framebuffer->GetImGuiTextureID(), { 300, 200});
				ImGui::End();
			}
			ImGui::End();
		}
	}

	void MaterialLibraryInterface::Reset()
	{
		m_Textures.clear();
		m_Buffer = {};
		m_UBO = {};

		// Reset image descriptors
		Texture* whiteTex = GraphicsContext::GetSingleton()->GetWhiteTexture();
		m_Data.Pipeline->UpdateSampler(whiteTex, 5);
		m_Data.Pipeline->UpdateSampler(whiteTex, 6);
		m_Data.Pipeline->UpdateSampler(whiteTex, 7);
		m_Data.Pipeline->UpdateSampler(whiteTex, 8);
		m_Data.Pipeline->UpdateSampler(whiteTex, 9);
	}

	void MaterialLibraryInterface::RenderImage()
	{
		auto& textures = m_MaterialCI.GetTexturesInfo();
		for (const auto& [type, path] : textures)
		{
			LoadTexture(path, type);
		}

		m_UBO.camPos = m_Data.Camera->GetPosition();
		m_Data.Pipeline->SubmitBuffer(m_BindingPoint, sizeof(UniformBuffer), &m_UBO);

		m_Data.Pipeline->BeginCommandBuffer();
		m_Data.Pipeline->BeginRenderPass();
		{
			Mesh* mesh = nullptr;
			switch (m_GeometryType)
			{
			case 0: mesh = GraphicsContext::GetSingleton()->GetBoxMesh(); break;
			case 1: mesh = GraphicsContext::GetSingleton()->GetSphereMesh(); break;
			case 2: mesh = GraphicsContext::GetSingleton()->GetCapsuleMesh(); break;
			}

			glm::mat4 viewProj = m_Data.Camera->GetProjection() * m_Data.Camera->GetViewMatrix();

			m_Data.Pipeline->SubmitPushConstant(ShaderType::Vertex, sizeof(glm::mat4), &viewProj);
			m_Data.Pipeline->DrawMeshIndexed(mesh);
		}
		m_Data.Pipeline->EndRenderPass();
		m_Data.Pipeline->EndCommandBuffer();

		Reset();
	}

	void MaterialLibraryInterface::InitPreviewRenderer()
	{
		// Editor Camera
		{
			EditorCameraCreateInfo info{};
			info.FOV = 35;
			m_Data.Camera = std::make_shared<EditorCamera>(&info);
		}

		// Framebuffer
		{
			Ref<Framebuffer> fb = std::make_shared<Framebuffer>();

			FramebufferSpecification framebufferCI = {};
			framebufferCI.Width = 380;
			framebufferCI.Height = 200;
			framebufferCI.bResizable = false;
			framebufferCI.bUsedByImGui = true;
			framebufferCI.NumSubpassDependencies = 0;
			framebufferCI.Attachments = { FramebufferAttachment(AttachmentFormat::Color, true) };
			framebufferCI.eMSAASampels = MSAASamples::SAMPLE_COUNT_MAX_SUPPORTED;

			Framebuffer::Create(framebufferCI, fb.get());
			m_Data.Framebuffer = fb;
		}

		// Pipeline
		{
			Ref<GraphicsPipeline> pipeline = std::make_shared<GraphicsPipeline>();

			BufferLayout mainLayout =
			{
				{ DataTypes::Float3, "aPos" },
				{ DataTypes::Float3, "aNormal" },
				{ DataTypes::Float3, "aTangent" },
				{ DataTypes::Float2, "aUV" },
				{ DataTypes::Int4,   "aBoneIDs"},
				{ DataTypes::Float4, "aWeight"}
			};

			VertexInputInfo vertexMain(sizeof(PBRVertex), mainLayout);

			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "assets/shaders/PBR_Preview.vert";
				shaderCI.FilePaths[ShaderType::Fragment] = "assets/shaders/PBR_Preview.frag";
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { vertexMain };
				DynamicPipelineCI.PipelineName = "PBR_Preview";
				DynamicPipelineCI.ShaderCreateInfo = shaderCI;
				DynamicPipelineCI.pTargetFramebuffer = m_Data.Framebuffer.get();
			}

			auto result = pipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);

			pipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetIrradianceImageInfo());
			pipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetBRDFLUTImageInfo());
			pipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetPrefilteredCubeImageInfo());

			m_Data.Pipeline = pipeline;
		}
	}

	void MaterialLibraryInterface::LoadTexture(const std::string& filePath, MaterialTexture type)
	{
		switch (type)
		{
		case MaterialTexture::Albedro: 
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				m_UBO.useAlbedro = true;
				m_Data.Pipeline->UpdateSampler(tex.get(), 5);
				m_Textures.push_back(tex);
			}

			break;
		}
		case MaterialTexture::Normal:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				m_UBO.useNormal = true;
				m_Data.Pipeline->UpdateSampler(tex.get(), 6);
				m_Textures.push_back(tex);
			}

			break;
		}
		case MaterialTexture::Metallic:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				m_UBO.useMetallic = true;
				m_Data.Pipeline->UpdateSampler(tex.get(), 7);
				m_Textures.push_back(tex);
			}

			break;
		}
		case MaterialTexture::Roughness:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				m_UBO.useRoughness = true;
				m_Data.Pipeline->UpdateSampler(tex.get(), 8);
				m_Textures.push_back(tex);
			}

			break;
		}
		case MaterialTexture::AO:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				m_UBO.useAO = true;
				m_Data.Pipeline->UpdateSampler(tex.get(), 9);
				m_Textures.push_back(tex);
			}

			break;
		}
		}
	}

	void MaterialLibraryInterface::DrawTextureInfo(const char* header, std::string& outString, std::string& dummy)
	{
		std::string id = header + std::string("add");
		ImGui::PushID(id.c_str());
		if (ImGui::Extensions::InputRawString(header, dummy, header, 100.0f, true))
			outString = dummy;

		ImGui::SameLine();
		if (ImGui::ImageButton(m_FolderTexture->GetImGuiTexture(), { 15, 15 }))
		{
			const auto& result = Utils::OpenFile("");
			if (result.has_value())
				dummy = result.value();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
			{
				std::string path;
				std::filesystem::path* p = (std::filesystem::path*)payload->Data;

				if (EditorLayer::FileExtensionCheck(p, ".png", path))
					dummy = path;

				if (EditorLayer::FileExtensionCheck(p, ".jpg", path))
					dummy = path;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		if (ImGui::ImageButton(m_RemoveTexture->GetImGuiTexture(), { 15, 15 }))
		{
			dummy = "";
		}
		
		outString = dummy;
		ImGui::PopID();
	}
}