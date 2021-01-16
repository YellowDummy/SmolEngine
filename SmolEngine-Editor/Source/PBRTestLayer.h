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

	struct Light
	{
		glm::vec4 Pos = glm::vec4(1.0f);
		glm::vec4 Color = glm::vec4(1.0f);
		float intensity = 100;
	};

	class PBRTestLayer : public Layer
	{
	public:

		PBRTestLayer()
			:Layer("VulkanTestLayer") {}

		~PBRTestLayer() {}

		/// Main

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		void OnAttach() override;

		void OnDetach() override;


		void BuildTestCommandBuffer();

	private:

		Ref<Texture> m_Tetxure1;
		Ref<Texture> m_Tetxure2;
		Ref<Texture> m_Tetxure3;
		Ref<Texture> m_Tetxure4;

		Ref<Mesh> m_TestMesh = nullptr;
		Ref<GraphicsPipeline> m_Pipeline = nullptr;

		Ref<Framebuffer> m_FrameBuffer;

		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::vec3 rot = glm::vec3(0.0f);

		Ref<EditorCamera> m_EditorCamera = nullptr;

		glm::vec4 m_AddColor = glm::vec4(1);

		Light m_Light = {};
		Light m_Light2 = {};
	};
}