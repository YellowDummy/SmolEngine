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

	void MaterialLibraryInterface::OpenExisting(const std::string& path)
	{
		if (std::filesystem::exists(path))
		{
			MaterialLibrary::GetSinglenton()->Load(path, m_MaterialCI);
			m_CurrentFilePath = path;
			RenderImage();
		}
	}

	void MaterialLibraryInterface::OpenNew(const std::string& path)
	{
		m_MaterialCI = {};
		m_CurrentFilePath = path;
	}

	void MaterialLibraryInterface::Close()
	{
		m_MaterialCI = {};
		m_CurrentFilePath = "";
	}

	void MaterialLibraryInterface::Update()
	{
		ImGui::NewLine();
		ImGui::SetCursorPosX( (ImGui::GetWindowWidth() / 2.0f) - 50.0f);
		ImGui::Text("Material Viewer");
		ImGui::Separator();

		ImGui::BeginChild("MaterialViewerWIndow");
		ImGui::NewLine();
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();

			auto& textures = m_MaterialCI.GetTexturesInfo();
			if (DrawTextureInfo("Albedro", textures[MaterialTexture::Albedro], "Albedro"))
				ApplyChanges();

			if (DrawTextureInfo("Normal", textures[MaterialTexture::Normal], "Normal"))
				ApplyChanges();

			if (DrawTextureInfo("Metallic", textures[MaterialTexture::Metallic], "Metalness"))
				ApplyChanges();

			if (DrawTextureInfo("Roughness", textures[MaterialTexture::Roughness], "Roughness"))
				ApplyChanges();

			if (DrawTextureInfo("AO", textures[MaterialTexture::AO], "AO"))
				ApplyChanges();
		}

		ImGui::NewLine();
		if (ImGui::CollapsingHeader("Attributes", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();

			if (ImGui::Extensions::InputFloat("Roughness", *m_MaterialCI.GetRoughness()))
				ApplyChanges();

			if (ImGui::Extensions::InputFloat("Metalness", *m_MaterialCI.GetMetalness()))
				ApplyChanges();
		}

		ImGui::EndChild();
		ImGui::SetCursorPosY(620);
		ImGui::Extensions::Text("Preview", "");
		ImGui::Separator();
		ImGui::Image(m_Data.Framebuffer->GetImGuiTextureID(), { 410, 280 });
	}

	void MaterialLibraryInterface::Save()
	{
		if (!m_CurrentFilePath.empty())
		{
			std::string path = m_CurrentFilePath;
			MaterialLibrary::GetSinglenton()->Save(path, m_MaterialCI);
			Close();
			OpenExisting(path);
		}
	}

	std::string MaterialLibraryInterface::GetCurrentPath() const
	{
		return m_CurrentFilePath;
	}

	void MaterialLibraryInterface::Reset()
	{
		m_Textures.clear();
		m_UBO = {};

		// Reset image descriptors
		Texture* whiteTex = GraphicsContext::GetSingleton()->GetWhiteTexture();
		m_Data.Pipeline->UpdateSampler(whiteTex, 5);
		m_Data.Pipeline->UpdateSampler(whiteTex, 6);
		m_Data.Pipeline->UpdateSampler(whiteTex, 7);
		m_Data.Pipeline->UpdateSampler(whiteTex, 8);
		m_Data.Pipeline->UpdateSampler(whiteTex, 9);
	}

	void MaterialLibraryInterface::ApplyChanges()
	{
		Save();
		RenderImage();
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

	bool MaterialLibraryInterface::DrawTextureInfo(const char* header, std::string& outString, const std::string& title)
	{
		bool used = false;
		std::string id = header + std::string("add");
		ImGui::PushID(id.c_str());
		ImGui::SetCursorPosX(12.0);

		if (ImGui::ImageButton(m_FolderTexture->GetImGuiTexture(), { 15, 15 }))
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
		if (ImGui::ImageButton(m_RemoveTexture->GetImGuiTexture(), { 15, 15 }))
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