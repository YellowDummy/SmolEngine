#pragma once

#include "SmolEngineCore.h"

#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"

#include "Renderer/Camera.h"
#include "Renderer/GraphicsPipeline.h"

namespace SmolEngine
{
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

		GraphicsPipeline m_GraphicsPipeline;
		Ref<Framebuffer> m_FrameBuffer;

		Ref<EditorCameraController> m_EditorCamera = nullptr;

		glm::vec3 m_AddColor = glm::vec3(1);
	};
}