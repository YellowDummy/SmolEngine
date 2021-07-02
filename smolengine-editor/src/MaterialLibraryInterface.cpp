#include "stdafx.h"
#include "MaterialLibraryInterface.h"
#include "EditorLayer.h"
#include "ImGuiExtension.h"

#include "ECS/Systems/JobsSystem.h"

#include <imgui/imgui.h>
#include <Frostium3D/Utils/Utils.h>
#include <Frostium3D/GraphicsPipeline.h>
#include <Frostium3D/GraphicsContext.h>
#include <Frostium3D/Vulkan/VulkanPBR.h>

namespace SmolEngine
{
	MaterialLibraryInterface::MaterialLibraryInterface()
	{
		m_TexturesLoader = TexturesLoader::Get();
		m_MaterialCI = new MaterialCreateInfo();
		m_Data = new PreviewRenderingData();
		InitPreviewRenderer();
	}

	MaterialLibraryInterface::~MaterialLibraryInterface()
	{

	}

	void MaterialLibraryInterface::OpenExisting(const std::string& path)
	{
		std::filesystem::path p(path);
		m_CurrentName = "Material: " + p.filename().stem().u8string();
		m_CurrentFilePath = path;

		ResetMaterial();
		MaterialLibrary::GetSinglenton()->Load(path, *m_MaterialCI);
		RenderImage();
	}

	void MaterialLibraryInterface::OpenNew(const std::string& path)
	{
		std::filesystem::path p(path);
		m_CurrentName = "Material: " + p.filename().stem().u8string();
		m_CurrentFilePath = path;
		ResetMaterial();
		RenderImage();
		Save();
	}

	void MaterialLibraryInterface::Close()
	{
		ResetMaterial();
		m_CurrentFilePath = "";
		m_Textures.clear();
	}

	void MaterialLibraryInterface::Update()
	{
		ImGui::NewLine();
		ImGui::Text(m_CurrentName.c_str());
		ImGui::Separator();

		ImGui::NewLine();
		ImGui::BeginChild("MaterialViewerWIndow");
		if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();

			if (DrawTextureInfo(m_MaterialCI->AlbedroPath, "Albedro"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->NormalPath, "Normal"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->MetallnessPath, "Metalness"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->RoughnessPath, "Roughness"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->AOPath, "AO"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->EmissivePath, "Emissive"))
				ApplyChanges();

			ImGui::NewLine();
		}
		if (ImGui::CollapsingHeader("Attributes", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();

			if (ImGui::Extensions::InputFloat("Roughness", m_MaterialCI->Roughness))
				ApplyChanges();
			if (ImGui::Extensions::InputFloat("Metalness", m_MaterialCI->Metallness))
				ApplyChanges();
			if (ImGui::Extensions::InputFloat("Emission Strength", m_MaterialCI->EmissionStrength))
				ApplyChanges();
			if (ImGui::Extensions::ColorInput3("Albedro", m_MaterialCI->AlbedroColor, 130, "AlbedroInput"))
				ApplyChanges();

			ImGui::NewLine();
		}

		ImGui::EndChild();
		float posX = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(posX - 200);
		ImGui::Extensions::Text("Preview", "");
		ImGui::Separator();
		ImGui::Image(m_Data->Framebuffer->GetImGuiTextureID(), { 410, 280 });
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
		m_Textures.clear();

		*m_MaterialCI = {};
		m_Material.Roughness = 1.0f;
		m_Material.Metalness = 0.12f;
		m_Material.Albedro = glm::vec4(1.0f);

		m_Material.UseAlbedroTex = 0;
		m_Material.UseNormalTex = 0;
		m_Material.UseMetallicTex = 0;
		m_Material.UseRoughnessTex = 0;
		m_Material.UseEmissiveTex = 0;
		m_Material.UseAOTex = 0;
	}

	void MaterialLibraryInterface::ApplyChanges()
	{
		RenderImage();
		Save();
	}

	void MaterialLibraryInterface::RenderImage()
	{
		Texture* whiteTex = GraphicsContext::GetSingleton()->GetWhiteTexture();
		m_Data->Pipeline->UpdateSamplers({ whiteTex }, 5);
		m_Data->Pipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetSingleton()->GetIrradianceImageInfo());
		m_Data->Pipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetSingleton()->GetBRDFLUTImageInfo());
		m_Data->Pipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetSingleton()->GetPrefilteredCubeImageInfo());

		std::unordered_map<MaterialTexture, std::string*> texture_infos;
		std::vector<Texture*> textures;

		m_Material.Metalness = m_MaterialCI->Metallness;
		m_Material.Roughness = m_MaterialCI->Roughness;
		m_Material.EmissionStrength = m_MaterialCI->EmissionStrength;
		m_Material.Albedro = glm::vec4(m_MaterialCI->AlbedroColor, 1.0f);
		m_Material.UseAlbedroTex = false;
		m_Material.UseNormalTex = false;
		m_Material.UseMetallicTex = false;
		m_Material.UseRoughnessTex = false;
		m_Material.UseEmissiveTex = false;
		m_Material.UseAOTex = false;

		m_MaterialCI->GetTextures(texture_infos);
		for (const auto& [type, path] : texture_infos)
		{
			LoadTexture(*path, type, textures);
		}

		m_Data->Pipeline->UpdateSamplers(textures, 5);
		m_Data->Pipeline->SubmitBuffer(m_BindingPoint, sizeof(PBRMaterial), &m_Material);
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

			struct pc
			{
				glm::mat4 viewProj;
				glm::vec3 camPos;
			} puch_constant;

			puch_constant.viewProj = m_Data->Camera->GetProjection() * m_Data->Camera->GetViewMatrix();
			puch_constant.camPos = m_Data->Camera->GetPosition();

			m_Data->Pipeline->SubmitPushConstant(ShaderType::Vertex, sizeof(pc), &puch_constant);
			m_Data->Pipeline->DrawMeshIndexed(mesh);
		}
		m_Data->Pipeline->EndRenderPass();
		m_Data->Pipeline->EndCommandBuffer();
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
				DynamicPipelineCI.TargetFramebuffers = { m_Data->Framebuffer.get() };
			}

			auto result = pipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);
			m_Data->Pipeline = pipeline;
		}
	}

	void MaterialLibraryInterface::LoadTexture(const std::string& filePath, MaterialTexture type, std::vector<Texture*>& textures)
	{
		uint32_t index = static_cast<uint32_t>(textures.size());
		Texture* tex = nullptr;

		auto& it = m_Textures.find(filePath);
		if (it == m_Textures.end())
		{
			Ref<Texture> tex_ = std::make_shared<Texture>();
			Texture::Create(filePath, tex_.get(), TextureFormat::R8G8B8A8_UNORM, true, true);
			m_Textures[filePath] = tex_;
			tex = tex_.get();
		}
		else { tex = it->second.get(); }

		switch (type)
		{
		case MaterialTexture::Albedro: 
		{
			if (!filePath.empty())
			{
				m_Material.UseAlbedroTex = true;
				m_Material.AlbedroTexIndex = index;
				textures.push_back(tex);
			}

			break;
		}
		case MaterialTexture::Normal:
		{
			if (!filePath.empty())
			{
				m_Material.UseNormalTex = true;
				m_Material.NormalTexIndex = index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::Metallic:
		{
			if (!filePath.empty())
			{
				m_Material.UseMetallicTex = true;
				m_Material.MetallicTexIndex= index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::Roughness:
		{
			if (!filePath.empty())
			{
				m_Material.UseRoughnessTex = true;
				m_Material.RoughnessTexIndex = index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::AO:
		{
			if (!filePath.empty())
			{
				m_Material.UseAOTex = true;
				m_Material.AOTexIndex = index;
				textures.emplace_back(tex);
			}

			break;
		}
		case MaterialTexture::Emissive:
		{
			if (!filePath.empty())
			{
				m_Material.UseEmissiveTex = true;
				m_Material.EmissiveTexIndex= index;
				textures.emplace_back(tex);
			}

			break;
		}
		}
	}

	bool MaterialLibraryInterface::DrawTextureInfo(std::string& texture_path, const std::string& title)
	{
		bool used = false;
		std::string id = title + std::string("draw_info");
		ImGui::PushID(id.c_str());

		float posX = 12.0f;
		ImGui::SetCursorPosX(posX);

		Texture* icon = nullptr;
		auto& it = m_Textures.find(texture_path);
		if (it != m_Textures.end())
			icon = it->second.get();
		else { icon = &m_TexturesLoader->m_BackgroundIcon; }

		ImGui::Image(icon->GetImGuiTexture(), { 60, 60 });
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
			{
				std::string& path = *(std::string*)payload->Data;
				if (EditorLayer::FileExtensionCheck(path, ".png") || EditorLayer::FileExtensionCheck(path, ".jpg"))
				{
					auto& it = m_Textures.find(path);
					if (it == m_Textures.end())
					{
						texture_path = path;
						used = true;

						Ref<Texture> tex = std::make_shared<Texture>();
						Texture::Create(path, tex.get(), TextureFormat::R8G8B8A8_UNORM, true, true);
						m_Textures[path] = tex;
					}
					else
					{
						texture_path = path;
						used = true;
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			texture_path = "";
			m_Textures.erase(texture_path);
			used = true; 
		}

		ImGui::SameLine();
		if (texture_path.empty())
		{
			std::string name = title + ": None";
			ImGui::TextUnformatted(name.c_str());
		}
		else
		{
			std::filesystem::path p(texture_path);
			std::string name = title + ": " + p.filename().filename().u8string();
			ImGui::TextUnformatted(name.c_str());
		}

		ImGui::PopID();
		return used;
	}
}