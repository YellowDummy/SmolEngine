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
	MaterialLibraryInterface::MaterialLibraryInterface(TexturesLoader* loader)
	{
		m_TexturesLoader = loader;
		m_MaterialCI = new MaterialCreateInfo();
		m_UBO = new MaterialLibraryInterface::UniformBuffer();
		m_Data = new PreviewRenderingData();
		InitPreviewRenderer();
	}

	MaterialLibraryInterface::~MaterialLibraryInterface()
	{

	}

	void MaterialLibraryInterface::OpenExisting(const std::string& path)
	{
		if (std::filesystem::exists(path))
		{
			ResetMaterial();
			MaterialLibrary::GetSinglenton()->Load(path, *m_MaterialCI);
			m_CurrentFilePath = path;
			RenderImage();
		}
	}

	void MaterialLibraryInterface::OpenNew(const std::string& path)
	{
		ResetMaterial();
		m_CurrentFilePath = path;
	}

	void MaterialLibraryInterface::Close()
	{
		ResetMaterial();
		m_CurrentFilePath = "";
	}

	void MaterialLibraryInterface::Update()
	{
		ImGui::NewLine();
		ImGui::SetCursorPosX( (ImGui::GetWindowWidth() / 2.0f) - 50.0f);
		ImGui::Text("Material Viewer");
		ImGui::Separator();

		bool modified = false;
		ImGui::BeginChild("MaterialViewerWIndow");
		ImGui::NewLine();
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();

			if (DrawTextureInfo("Albedro", m_MaterialCI->AlbedroPath, "Albedro"))
				modified = true;

			if (DrawTextureInfo("Normal", m_MaterialCI->NormalPath, "Normal"))
				modified = true;

			if (DrawTextureInfo("Metallic", m_MaterialCI->MetallnessPath, "Metalness"))
				modified = true;

			if (DrawTextureInfo("Roughness", m_MaterialCI->RoughnessPath, "Roughness"))
				modified = true;

			if (DrawTextureInfo("Emissive", m_MaterialCI->EmissivePath, "Emissive"))
				modified = true;

			if (DrawTextureInfo("AO", m_MaterialCI->AOPath, "AO"))
				modified = true;

			if (DrawTextureInfo("Height", m_MaterialCI->HeightPath, "Height"))
				modified = true;
		}

		ImGui::NewLine();
		if (ImGui::CollapsingHeader("Attributes", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();

			if (ImGui::Extensions::InputFloat("Roughness", m_MaterialCI->Roughness))
				modified = true;
			if (ImGui::Extensions::InputFloat("Metalness", m_MaterialCI->Metallness))
				modified = true;
			if (ImGui::Extensions::ColorInput3("Albedro", m_MaterialCI->AlbedroColor, 130, "AlbedroInput"))
				modified = true;

			if (ImGui::Button("Apply Changes"))
				Save();
		}

		ImGui::EndChild();
		ImGui::SetCursorPosY(620);
		ImGui::Extensions::Text("Preview", "");
		ImGui::Separator();
		ImGui::Image(m_Data->Framebuffer->GetImGuiTextureID(), { 410, 280 });

		if (modified) 
		{
			ApplyChanges();
		}
	}

	void MaterialLibraryInterface::Save()
	{
		if (!m_CurrentFilePath.empty())
		{
			std::string path = m_CurrentFilePath;
			MaterialLibrary::GetSinglenton()->Save(path, *m_MaterialCI);
		}
	}

	std::string MaterialLibraryInterface::GetCurrentPath() const
	{
		return m_CurrentFilePath;
	}

	void MaterialLibraryInterface::ResetMaterial()
	{
		*m_MaterialCI = {};
		m_UBO->material.Roughness = 1.0f;
		m_UBO->material.Metalness = 0.12f;
		m_UBO->material.Albedro = glm::vec4(1.0f);

		m_UBO->material.UseAlbedroTex = false;
		m_UBO->material.UseNormalTex = false;
		m_UBO->material.UseMetallicTex = false;
		m_UBO->material.UseRoughnessTex = false;
		m_UBO->material.UseEmissiveTex = false;
		m_UBO->material.UseHeightTex = false;
		m_UBO->material.UseAOTex = false;
	}

	void MaterialLibraryInterface::ApplyChanges()
	{
		RenderImage();
	}

	void MaterialLibraryInterface::RenderImage()
	{
		std::unordered_map<MaterialTexture, std::string*> texture_infos;
		std::vector<Ref<Texture>> textures;

		m_UBO->material.Metalness = m_MaterialCI->Metallness;
		m_UBO->material.Roughness = m_MaterialCI->Roughness;
		m_UBO->material.Albedro = glm::vec4(m_MaterialCI->AlbedroColor, 1.0f);
		m_UBO->camPos = m_Data->Camera->GetPosition();

		m_MaterialCI->GetTextures(texture_infos);
		for (const auto& [type, path] : texture_infos)
		{
			LoadTexture(*path, type, *m_UBO, textures);
		}

		m_Data->Pipeline->SubmitBuffer(m_BindingPoint, sizeof(UniformBuffer), m_UBO);
		m_Data->Pipeline->BeginCommandBuffer();
		m_Data->Pipeline->BeginRenderPass();
		{
			Mesh* mesh = nullptr;
			switch (m_GeometryType)
			{
			case 0: mesh = GraphicsContext::GetSingleton()->GetBoxMesh(); break;
			case 1: mesh = GraphicsContext::GetSingleton()->GetSphereMesh(); break;
			case 2: mesh = GraphicsContext::GetSingleton()->GetCapsuleMesh(); break;
			}

			glm::mat4 viewProj = m_Data->Camera->GetProjection() * m_Data->Camera->GetViewMatrix();
			m_Data->Pipeline->SubmitPushConstant(ShaderType::Vertex, sizeof(glm::mat4), &viewProj);
			m_Data->Pipeline->DrawMeshIndexed(mesh);
		}
		m_Data->Pipeline->EndRenderPass();
		m_Data->Pipeline->EndCommandBuffer();

		Texture* whiteTex = GraphicsContext::GetSingleton()->GetWhiteTexture();
		m_Data->Pipeline->UpdateSamplers({ whiteTex }, 5);
	}

	void MaterialLibraryInterface::InitPreviewRenderer()
	{
		// Editor Camera
		{
			EditorCameraCreateInfo info{};
			info.FOV = 35;
			m_Data->Camera = std::make_shared<EditorCamera>(&info);
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
			m_Data->Framebuffer = fb;
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
				DynamicPipelineCI.pTargetFramebuffer = m_Data->Framebuffer.get();
			}

			auto result = pipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);

			pipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetIrradianceImageInfo());
			pipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetBRDFLUTImageInfo());
			pipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetPrefilteredCubeImageInfo());

			m_Data->Pipeline = pipeline;
		}
	}

	void MaterialLibraryInterface::LoadTexture(const std::string& filePath, MaterialTexture type, UniformBuffer& ubo, std::vector<Ref<Texture>>& textures)
	{
		uint32_t index = static_cast<uint32_t>(textures.size());

		switch (type)
		{
		case MaterialTexture::Albedro: 
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				ubo.material.UseAlbedroTex = true;
				ubo.material.AlbedroTexIndex = index;
				textures.push_back(tex);
			}

			break;
		}
		case MaterialTexture::Normal:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				ubo.material.UseNormalTex= true;
				ubo.material.NormalTexIndex = index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::Metallic:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				ubo.material.UseMetallicTex= true;
				ubo.material.MetallicTexIndex= index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::Roughness:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				ubo.material.UseRoughnessTex = true;
				ubo.material.RoughnessTexIndex = index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::AO:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				ubo.material.UseAOTex = true;
				ubo.material.AOTexIndex = index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::Emissive:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				ubo.material.UseEmissiveTex = true;
				ubo.material.EmissiveTexIndex= index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::Height:
		{
			if (!filePath.empty())
			{
				Ref<Texture> tex = std::make_shared<Texture>();
				Texture::Create(filePath, tex.get());

				ubo.material.UseHeightTex = true;
				ubo.material.HeightTexIndex = index;
				textures.emplace_back(tex);
			}

			break;
		}
		}
	}

	bool MaterialLibraryInterface::DrawTextureInfo(const char* header, std::string& outString, const std::string& title)
	{
		bool used = false;
		std::string id = header + std::string("add");
		ImGui::PushID(id.c_str());
		ImGui::SetCursorPosX(12.0);

		if (ImGui::ImageButton(m_TexturesLoader->m_FolderButton.GetImGuiTexture(), { 15, 15 }))
		{
			const auto& result = Utils::OpenFile("");
			if (result.has_value())
			{
				outString = result.value();
				used = true;
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
			{
				std::string path;
				std::filesystem::path* p = (std::filesystem::path*)payload->Data;

				if (EditorLayer::FileExtensionCheck(p, ".png", path))
				{
					outString = path;
					used = true;
				}

				if (EditorLayer::FileExtensionCheck(p, ".jpg", path))
				{
					outString = path;
					used = true;
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		if (ImGui::ImageButton(m_TexturesLoader->m_RemoveButton.GetImGuiTexture(), { 15, 15 }))
		{
			outString = "";
			used = true;
		}

		ImGui::SameLine();
		if (outString.empty())
			ImGui::TextUnformatted(title.c_str());
		else
		{
			std::filesystem::path p(outString);
			ImGui::TextUnformatted(p.filename().filename().u8string().c_str());
		}

		ImGui::PopID();
		return used;
	}
}