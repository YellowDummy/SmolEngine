#pragma once

#include "SmolEngineCore.h"

#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/EditorCamera.h"

#include "Renderer/Camera.h"
#include "Renderer/GraphicsPipeline.h"

namespace SmolEngine
{
	class Mesh;

	struct PBR
	{
		float ao = 1.0f;
		float roughness = 0.2f;
		float metallic = 0.6f;

		glm::vec3 albedo = glm::vec3(0.4f);
		glm::vec3 camPos = glm::vec3(0.0f);

		glm::vec3 lightPositions[4];
		glm::vec3 lightColor[4];

	};

	class VulkanTestLayer : public Layer
	{
	public:

		VulkanTestLayer()
			:Layer("VulkanTestLayer") {}

		~VulkanTestLayer() {}

		/// Main

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		void OnAttach() override;

		void OnDetach() override;


		void BuildTestCommandBuffer();

	private:

		Ref<Texture2D> m_Tetxure1;
		Ref<Texture2D> m_Tetxure2;
		Ref<Texture2D> m_Tetxure3;
		Ref<Texture2D> m_Tetxure4;

		Ref<Mesh> m_TestMesh;

		Ref<Framebuffer> m_FrameBuffer;

		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::vec3 rot = glm::vec3(0.0f);

		Ref<EditorCamera> m_EditorCamera = nullptr;

		glm::vec4 m_AddColor = glm::vec4(1);

		PBR m_PBR = {};
	};
}