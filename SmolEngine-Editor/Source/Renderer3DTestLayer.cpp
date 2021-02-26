#include "stdafx.h"
#include "Renderer3DTestLayer.h"

#include "Core/FilePaths.h"

#include "Renderer/EditorCamera.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/Mesh.h"
#include "Renderer/MaterialLibrary.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace SmolEngine
{
	void Renderer3DTestLayer::OnAttach()
	{
		GraphicsContextInitInfo GCInfo = {};
		GCInfo.bMSAA = true;
		GCInfo.bTargetsSwapchain = true;
		assert(GraphicsContext::Init(GCInfo) == true);

		// Editor Camera
		EditorCameraCreateInfo cameraCI = {};
		{
			m_EditorCamera = std::make_shared<EditorCamera>(&cameraCI);
		}

		// Create Materials

		int32_t floorID = 0;
		int32_t chairID = 0;
		int32_t defaultID = 0;
		int32_t glassID = 0;
		int32_t sponzaBase = 0;
		int32_t sponzaRoof = 0;
		int32_t sponzaLion = 0;
		int32_t sponzaFlowers = 0;
		int32_t sponzaVase = 0;
		int32_t sponzaCurtainRed = 0;
		int32_t sponzaCurtainGreen = 0;
		int32_t sponzaCurtainBlue = 0;
		int32_t sponzaThorn = 0;
		int32_t sponzaBrazier = 0;
		int32_t sponzaChain = 0;
		int32_t sponzaFlagPole = 0;

		// Default
		MaterialCreateInfo MaterialCI = {};
		MaterialCI.Name = "Default";

		defaultID = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// Test
		MaterialCI = {};
		MaterialCI.Name = "Test";
		MaterialCI.Metallic = 1.0f;
		MaterialCI.Roughness = 0.2f;

		m_TestMaterial = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// Chair
		MaterialCI = {};
		MaterialCI.Name = "Chair";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "WoodenChair_01_16-bit_Diffuse.png";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "WoodenChair_01_16-bit_Normal.png";
		MaterialCI.Textures[MaterialTexture::Metallic] = Resources + "WoodenChair_01_16-bit_Metallic.png";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "WoodenChair_01_16-bit_Roughness.png";
		chairID = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaBase
		MaterialCI = {};
		MaterialCI.Name = "SponzaBase";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Sponza_Bricks_a_Albedo.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Sponza_Bricks_a_Normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Sponza_Bricks_a_Roughness.PNG";
		MaterialCI.Metallic = 0.2f;

		sponzaBase = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaRoof
		MaterialCI = {};
		MaterialCI.Name = "SponzaRoof";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Sponza_Roof_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Sponza_Roof_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Sponza_Roof_roughness.PNG";
		MaterialCI.Metallic = 0.0;

		sponzaRoof = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaLion
		MaterialCI = {};
		MaterialCI.Name = "SponzaLion";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Lion_Albedo.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Lion_Normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Lion_Roughness.PNG";
		MaterialCI.Metallic = 1.0;

		sponzaLion = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaFlowers
		MaterialCI = {};
		MaterialCI.Name = "SponzaFlowers";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/VasePlant_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/VasePlant_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/VasePlant_roughness.PNG";
		MaterialCI.Metallic = 0.0;

		sponzaFlowers = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaVase
		MaterialCI = {};
		MaterialCI.Name = "SponzaVase";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Vase_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Vase_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Vase_roughness.PNG";
		MaterialCI.Metallic = 1.0;

		sponzaVase = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaCurtainRed
		MaterialCI = {};
		MaterialCI.Name = "SponzaCurtainRed";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Sponza_Curtain_Red_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Sponza_Curtain_Red_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Sponza_Curtain_roughness.PNG";
		MaterialCI.Textures[MaterialTexture::Metallic] = Resources + "/Test/Sponza_Curtain_metallic.PNG";

		sponzaCurtainRed = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaCurtainGreen
		MaterialCI = {};
		MaterialCI.Name = "SponzaCurtainGreen";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Sponza_Curtain_Green_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Sponza_Curtain_Green_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Sponza_Curtain_roughness.PNG";
		MaterialCI.Textures[MaterialTexture::Metallic] = Resources + "/Test/Sponza_Curtain_metallic.PNG";

		sponzaCurtainGreen = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaCurtainBlue
		MaterialCI = {};
		MaterialCI.Name = "SponzaCurtainBlue";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Sponza_Curtain_Blue_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Sponza_Curtain_Blue_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Sponza_Curtain_roughness.PNG";
		MaterialCI.Textures[MaterialTexture::Metallic] = Resources + "/Test/Sponza_Curtain_metallic.PNG";

		sponzaCurtainBlue = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaThorn
		MaterialCI = {};
		MaterialCI.Name = "SponzaThorn";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Sponza_Thorn_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Sponza_Thorn_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Sponza_Thorn_roughness.PNG";
		MaterialCI.Metallic = 0.0;

		sponzaThorn =  MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		//SponzaBrazier
		MaterialCI = {};
		MaterialCI.Name = "SponzaBrazier";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/VaseHanging_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/VaseHanging_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/VaseHanging_roughness.PNG";
		MaterialCI.Metallic = 0.0;

		sponzaBrazier = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// SponzaChain
		MaterialCI = {};
		MaterialCI.Name = "SponzaChain";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/ChainTexture_Albedo.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/ChainTexture_Normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/ChainTexture_Roughness.PNG";
		MaterialCI.Textures[MaterialTexture::Metallic] = Resources + "/Test/ChainTexture_Metallic.PNG";

		sponzaChain = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);

		// FlagPole
		MaterialCI = {};
		MaterialCI.Name = "SponzaFlagPole";
		MaterialCI.Textures[MaterialTexture::Albedro] = Resources + "/Test/Sponza_FlagPole_diffuse.PNG";
		MaterialCI.Textures[MaterialTexture::Normal] = Resources + "/Test/Sponza_FlagPole_normal.PNG";
		MaterialCI.Textures[MaterialTexture::Roughness] = Resources + "/Test/Sponza_FlagPole_roughness.PNG";
		MaterialCI.Metallic = 0.5f;

		sponzaFlagPole = MaterialLibrary::GetSinglenton()->Add(&MaterialCI);
		MaterialLibrary::GetSinglenton()->Save();

		Renderer::UpdateMaterials();

		// Load Models
		m_ChairMesh = Mesh::Create(Resources + "WoodenChair_01.FBX");
		m_SponzaMesh = Mesh::Create(Resources + "sponza.glb");
		m_CubeMesh = Mesh::Create(Resources + "cube.glb");

		// Set Materials
		m_ChairMesh->SetMaterialID(chairID, true);
		m_CubeMesh->SetMaterialID(defaultID, true);

		m_SponzaMesh->SetMaterialID(sponzaBase);

		Ref<Mesh> roof = m_SponzaMesh->FindSubMeshByName("Roof");
		if (roof)
			roof->SetMaterialID(sponzaRoof);

		Ref<Mesh> lion = m_SponzaMesh->FindSubMeshByName("Lions");
		if (lion)
			lion->SetMaterialID(sponzaLion);

		Ref<Mesh> flowers = m_SponzaMesh->FindSubMeshByName("Flowers");
		if (flowers)
			flowers->SetMaterialID(sponzaFlowers);

		Ref<Mesh> vase = m_SponzaMesh->FindSubMeshByName("Flowerpots");
		if (vase)
			vase->SetMaterialID(sponzaVase);

		Ref<Mesh> curtainRed = m_SponzaMesh->FindSubMeshByName("Curtains (red)");
		if (curtainRed)
			curtainRed->SetMaterialID(sponzaCurtainRed);

		Ref<Mesh> curtainGreen = m_SponzaMesh->FindSubMeshByName("Curtains (green)");
		if (curtainGreen)
			curtainGreen->SetMaterialID(sponzaCurtainGreen);

		Ref<Mesh> curtainBlue = m_SponzaMesh->FindSubMeshByName("Curtains (blue)");
		if (curtainBlue)
			curtainBlue->SetMaterialID(sponzaCurtainBlue);

		Ref<Mesh> thorn = m_SponzaMesh->FindSubMeshByName("Shrub");
		if (thorn)
			thorn->SetMaterialID(sponzaThorn);

		Ref<Mesh> brazier = m_SponzaMesh->FindSubMeshByName("Braziers");
		if (brazier)
			brazier->SetMaterialID(sponzaBrazier);

		Ref<Mesh> chain = m_SponzaMesh->FindSubMeshByName("Brazier chains");
		if (chain)
			chain->SetMaterialID(sponzaChain);

		Ref<Mesh> rod = m_SponzaMesh->FindSubMeshByName("Rods");
		if (rod)
			rod->SetMaterialID(sponzaFlagPole);
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

		ImGui::Checkbox("Debug View", &m_EnableDebugView);
		if(m_EnableDebugView)
			ImGui::DragInt("CascadeIndex", &m_Cuscade, 0.1f, 0, 3);

		ImGui::NewLine();
		ImGui::InputFloat("Metallic", &MaterialLibrary::GetSinglenton()->GetMaterial(std::string("Test"))->m_MaterialProperties.PBRValues.x);
		ImGui::InputFloat("Roughness", &MaterialLibrary::GetSinglenton()->GetMaterial(std::string("Test"))->m_MaterialProperties.PBRValues.y);

		ImGui::InputFloat("Gamma", &m_Gamma);
		ImGui::InputFloat("Ambient", &m_Ambient);
		ImGui::InputFloat("Gamma", &m_Gamma);
		ImGui::InputFloat("Exposure", &m_Exposure);
		ImGui::DragFloat3("Pos", glm::value_ptr(m_Pos));
		ImGui::ColorPicker3("Color", glm::value_ptr(m_Color));
		ImGui::End();
	}

	void Renderer3DTestLayer::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		// States
		Renderer::SetGamma(m_Gamma);
		Renderer::SetExposure(m_Exposure);
		Renderer::SetActiveDebugView(m_EnableDebugView);

		static BeginSceneInfo info;
		info.view = m_EditorCamera->GetViewMatrix();
		info.proj = m_EditorCamera->GetProjection();
		info.pos = m_EditorCamera->GetPosition();
		info.nearClip = m_EditorCamera->GetNearClip();
		info.farClip = m_EditorCamera->GetFarClip();

		static DebugViewInfo debugView;
		debugView.bShowOmniCube = true;

		Renderer::BeginScene(info);
		{
			if (m_EnableDebugView)
				Renderer::SetDebugViewParams(debugView);

			//Renderer::SubmitPointLight({ 0, 0, 0 }, { 0.2, 0.3, 0.3, 1.0 }, 0.5f, 0.1f, 0.0080f);
			Renderer::SubmitDirectionalLight(m_Pos, m_Color);

			Renderer::SubmitMesh({ 0, -4, 0 }, { 0.0f, 0, 0 }, { 100, 1, 100 }, m_CubeMesh);
			Renderer::SubmitMesh({ 0, 20, -50 }, { 0, 0, 0 }, { 10, 10,10 }, m_CubeMesh, m_TestMaterial);

			Renderer::SubmitMesh({ 1, 1, 1 }, { 0, 0, 0 }, { 0.2, 0.2, 0.2 }, m_ChairMesh);
			Renderer::SubmitMesh({ -20, 1, 1 }, { 0, 0, 0 }, { 0.2, 0.2, 0.2 }, m_ChairMesh);
			Renderer::SubmitMesh({ 20, 1, 1 }, { 0, 0, 0 }, { 0.2, 0.2, 0.2 }, m_ChairMesh);

			Renderer::SubmitMesh({ 0, 50, -200 }, { 0, 0, 0 }, {5, 5,5 }, m_SponzaMesh);
		}
		Renderer::EndScene();
	}
}