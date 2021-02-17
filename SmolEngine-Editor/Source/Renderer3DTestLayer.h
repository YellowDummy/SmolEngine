#pragma once
#include "Core/Core.h"
#include "Core/Layer.h"
#include "Renderer/Renderer.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	class EditorCamera;
	class Mesh;

	class Renderer3DTestLayer : public Layer
	{
	public:

		void OnAttach() override;

		void OnDetach() override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		void OnUpdate(DeltaTime deltaTime) override;

	private:

		Ref<EditorCamera>   m_EditorCamera = nullptr;
		Ref<Mesh>           m_ChairMesh = nullptr;
		Ref<Mesh>           m_CubeMesh = nullptr;
		Ref<Mesh>           m_SponzaMesh = nullptr;

		glm::vec3           m_Pos = glm::vec3(-15.0, -15 * 0.5f, -15);
		glm::vec4           m_Color = glm::vec4(1);
		float               m_Ambient = 1.0f;
		float               m_Gamma = 2.5f;
		float               m_Exposure = 4.0f;
	};
}