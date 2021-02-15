#include "stdafx.h"
#include "Renderer3DTestLayer.h"

#include "Core/FilePaths.h"

#include "Renderer/EditorCamera.h"
#include "Renderer/Mesh.h"
#include "Renderer/MaterialLibrary.h"
#include <imgui/imgui.h>

namespace SmolEngine
{
	void Renderer3DTestLayer::OnAttach()
	{
		// Init Renderer
		Renderer::Init();

		// Editor Camera
		EditorCameraCreateInfo cameraCI = {};
		{
			m_EditorCamera = std::make_shared<EditorCamera>(&cameraCI);
			m_EditorCamera->SetFramebuffers({ Renderer::GetFramebuffer() });
		}

		// Create Materials
		MaterialCreateInfo materialCI = {};
		{
			materialCI.Name = "Chair";
			materialCI.Textures[MaterialTexture::Albedro] = Texture::Create(Resources + "WoodenChair_01_16-bit_Diffuse.png");
			materialCI.Textures[MaterialTexture::Normal] = Texture::Create(Resources + "WoodenChair_01_16-bit_Normal.png");
			materialCI.Textures[MaterialTexture::Metallic] = Texture::Create(Resources + "WoodenChair_01_16-bit_Metallic.png");
			materialCI.Textures[MaterialTexture::Roughness] = Texture::Create(Resources + "WoodenChair_01_16-bit_Roughness.png");
		}
		assert(MaterialLibrary::GetSinglenton()->Add(&materialCI) != -1);

		Renderer::UpdateMaterials();

		// Load Models
		m_CubeMesh = Mesh::Create(Resources + "cube.glb");
	}

	void Renderer3DTestLayer::OnDetach()
	{

	}

	void Renderer3DTestLayer::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);
	}

	void Renderer3DTestLayer::OnImGuiRender()
	{
		ImGui::Begin("Settings");
		ImGui::End();
	}

	void Renderer3DTestLayer::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		Renderer::BeginScene(m_EditorCamera->GetProjection(), m_EditorCamera->GetViewMatrix(), m_EditorCamera->GetPosition());
		{

		}
		Renderer::EndScene();
	}
}