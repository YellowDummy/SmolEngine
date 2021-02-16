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

		int32_t chairID = 0;
		int32_t defaultID = 0;

		// Default
		MaterialCreateInfo DefaultMaterialCI = {};
		DefaultMaterialCI.Name = "Default";

		defaultID = MaterialLibrary::GetSinglenton()->Add(&DefaultMaterialCI);

		// Chair
		MaterialCreateInfo ChairMaterialCI = {};
		ChairMaterialCI.Name = "Chair";
		ChairMaterialCI.Textures[MaterialTexture::Albedro] = Texture::Create(Resources + "WoodenChair_01_16-bit_Diffuse.png");
		ChairMaterialCI.Textures[MaterialTexture::Normal] = Texture::Create(Resources + "WoodenChair_01_16-bit_Normal.png");
		ChairMaterialCI.Textures[MaterialTexture::Metallic] = Texture::Create(Resources + "WoodenChair_01_16-bit_Metallic.png");
		ChairMaterialCI.Textures[MaterialTexture::Roughness] = Texture::Create(Resources + "WoodenChair_01_16-bit_Roughness.png");

		chairID = MaterialLibrary::GetSinglenton()->Add(&ChairMaterialCI);

		Renderer::UpdateMaterials();

		// Load Models
		m_ChairMesh = Mesh::Create(Resources + "WoodenChair_01.FBX");
		m_CubeMesh = Mesh::Create(Resources + "cube.glb");

		m_ChairMesh->SetMaterialID(chairID, true);
		m_CubeMesh->SetMaterialID(defaultID, true);
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
			Renderer::SubmitMesh({ 0, -5, 0 }, { 0, 0, 0 }, { 100, 1, 100 }, m_CubeMesh);
			Renderer::SubmitMesh({ 1, 1, 1 }, { 0, 0, 0 }, { 0.2, 0.2, 0.2 }, m_ChairMesh);
			Renderer::SubmitMesh({ -20, 1, 1 }, { 0, 0, 0 }, { 0.2, 0.2, 0.2 }, m_ChairMesh);
			Renderer::SubmitMesh({ 20, 1, 1 }, { 0, 0, 0 }, { 0.2, 0.2, 0.2 }, m_ChairMesh);
		}
		Renderer::EndScene();
	}
}