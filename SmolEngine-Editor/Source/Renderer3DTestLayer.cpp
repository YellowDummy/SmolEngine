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

		// Load Models
		m_ChairMesh = Mesh::Create(Resources + "WoodenChair_01.FBX");
		m_SponzaMesh = Mesh::Create(Resources + "sponza.glb");
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

		ImGui::Checkbox("Debug View", &m_EnableDebugView);
		if(m_EnableDebugView)
			ImGui::DragInt("CascadeIndex", &m_Cuscade, 0.1f, 0, 3);

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