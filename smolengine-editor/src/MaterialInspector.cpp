#include "stdafx.h"
#include "MaterialInspector.h"
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
	MaterialInspector::MaterialInspector()
	{
		m_TexturesLoader = TexturesLoader::Get();
		m_MaterialCI = new MaterialCreateInfo();
		m_Data = new PreviewRenderingData();
		InitPreviewRenderer();
	}

	MaterialInspector::~MaterialInspector()
	{

	}

	void MaterialInspector::OpenExisting(const std::string& path)
	{
		std::filesystem::path p(path);
		m_CurrentName = p.filename().stem().u8string();
		m_CurrentFilePath = path;

		ResetMaterial();
		m_MaterialCI->Load(path);
		RenderImage();
	}

	void MaterialInspector::OpenNew(const std::string& path)
	{
		std::filesystem::path p(path);
		m_CurrentName = p.filename().stem().u8string();
		m_CurrentFilePath = path;
		ResetMaterial();
		RenderImage();
		Save();
	}

	void MaterialInspector::Close()
	{
		m_CurrentFilePath = "";
	}

	void MaterialInspector::Update()
	{
		ImGui::NewLine();
		ImGui::BeginChild("MaterialViewerWIndow");

		{
			ImGui::SetCursorPosX(12);
			ImGui::TextWrapped("Preview: %s", m_CurrentName.c_str());
			ImGui::Separator();
			ImGui::SetCursorPosX(12);
			ImGui::Image(m_Data->Framebuffer->GetImGuiTextureID(), { 440, 280 });
			if (ImGui::IsItemHovered())
			{
				float my_tex_h = 280;
				float my_tex_w = 440;
				ImVec2 pos = ImGui::GetCursorScreenPos();
				auto& io = ImGui::GetIO();
				ImGui::BeginTooltip();
				float region_sz = 32.0f;
				float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
				float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
				float zoom = 5.0f;
				ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
				ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
				ImGui::Image(m_Data->Framebuffer->GetImGuiTextureID(), ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1);
				ImGui::EndTooltip();
			}
		}

		if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();

			if (DrawTextureInfo(m_MaterialCI->AlbedroTex, "Albedro"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->NormalTex, "Normal"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->MetallnessTex, "Metalness"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->RoughnessTex, "Roughness"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->AOTex, "AO"))
				ApplyChanges();

			if (DrawTextureInfo(m_MaterialCI->EmissiveTex, "Emissive"))
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
	}

	void MaterialInspector::Save()
	{
		if (!m_CurrentFilePath.empty())
		{
			std::string path = m_CurrentFilePath;
			m_MaterialCI->Save(path);
		}
	}

	std::string MaterialInspector::GetCurrentPath() const
	{
		return m_CurrentFilePath;
	}

	void MaterialInspector::ResetMaterial()
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

	void MaterialInspector::ApplyChanges()
	{
		RenderImage();
		Save();
	}

	void MaterialInspector::RenderImage()
	{
		Texture* whiteTex = GraphicsContext::GetSingleton()->GetWhiteTexture();
		m_Data->Pipeline->UpdateSamplers({ whiteTex }, 5);
		m_Data->Pipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetSingleton()->GetIrradianceImageInfo());
		m_Data->Pipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetSingleton()->GetBRDFLUTImageInfo());
		m_Data->Pipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetSingleton()->GetPrefilteredCubeImageInfo());

		std::unordered_map<MaterialTexture, TextureCreateInfo*> texture_infos;
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
		for (const auto& [type, info] : texture_infos)
		{
			LoadTexture(info, type, textures);
		}

		m_Data->Pipeline->UpdateSamplers(textures, 5);
		m_Data->Pipeline->SubmitBuffer(m_BindingPoint, sizeof(PBRMaterial), &m_Material);
		m_Data->Pipeline->BeginCommandBuffer();
		m_Data->Pipeline->BeginRenderPass();
		{
			Mesh* mesh = nullptr;
			switch (m_GeometryType)
			{
			case 0: mesh = GraphicsContext::GetSingleton()->GetDefaultMeshes()->Cube; break;
			case 1: mesh = GraphicsContext::GetSingleton()->GetDefaultMeshes()->Sphere; break;
			case 2: mesh = GraphicsContext::GetSingleton()->GetDefaultMeshes()->Torus; break;
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

	void MaterialInspector::InitPreviewRenderer()
	{
		// Editor Camera
		{
			EditorCameraCreateInfo info{};
			info.FOV = 35;
			info.WorldPos = glm::vec3(0, 0, -2);
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
			framebufferCI.bAutoSync = false;
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

	void MaterialInspector::LoadTexture(TextureCreateInfo* info, MaterialTexture type, std::vector<Texture*>& textures)
	{
		uint32_t index = static_cast<uint32_t>(textures.size());
		std::string filePath = info->FilePath;
		Texture* tex = nullptr;

		auto& it = m_Textures.find(filePath);
		if (it == m_Textures.end())
		{
			
			Ref<Texture> tex_ = std::make_shared<Texture>();
			info->bImGUIHandle = true;
			Texture::Create(info, tex_.get());
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

	bool MaterialInspector::DrawTextureInfo(TextureCreateInfo& texInfo, const std::string& title)
	{
		bool used = false;
		std::string id = title + std::string("draw_info");
		ImGui::PushID(id.c_str());

		float posX = 12.0f;
		ImGui::SetCursorPosX(posX);

		Texture* icon = nullptr;
		auto& it = m_Textures.find(texInfo.FilePath);
		if (it != m_Textures.end())
			icon = it->second.get();
		else { icon = &m_TexturesLoader->m_BackgroundIcon; }

		ImGui::Image(icon->GetImGuiTexture(), { 60, 60 });
		if (it != m_Textures.end())
		{
			if (ImGui::IsItemHovered())
			{
				float my_tex_h = 60;
				float my_tex_w = 60;
				ImVec2 pos = ImGui::GetCursorScreenPos();
				auto& io = ImGui::GetIO();
				ImGui::BeginTooltip();
				float region_sz = 32.0f;
				float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
				float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
				float zoom = 5.0f;
				ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
				ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
				ImGui::Image(icon->GetImGuiTexture(), ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1);
				ImGui::EndTooltip();
			}
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
			{
				std::string& path = *(std::string*)payload->Data;
				if (EditorLayer::FileExtensionCheck(path, ".s_image"))
				{
					texInfo.Load(path);
					auto& it = m_Textures.find(path);
					if (it == m_Textures.end())
					{
						used = true;
						texInfo.bImGUIHandle = true;

						Ref<Texture> tex = std::make_shared<Texture>();
						Texture::Create(&texInfo, tex.get());
						m_Textures[path] = tex;
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			texInfo = {};
			m_Textures.erase(texInfo.FilePath);
			used = true; 
		}

		ImGui::SameLine();
		if (texInfo.FilePath.empty())
		{
			std::string name = title + ": None";
			ImGui::TextUnformatted(name.c_str());
		}
		else
		{
			std::filesystem::path p(texInfo.FilePath);
			std::string name = title + ": " + p.filename().filename().u8string();
			ImGui::TextUnformatted(name.c_str());
		}

		ImGui::PopID();
		return used;
	}
}