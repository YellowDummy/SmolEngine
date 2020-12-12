#pragma once

#include "SmolEngineCore.h"

#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Framebuffer.h"

#include "Core/Renderer/Camera.h"
#include "Core/Renderer/GraphicsPipeline.h"

namespace SmolEngine
{
	class VulkanTestLayer : public Layer
	{
	public:

		VulkanTestLayer()
			:Layer("VulkanTestLayer") {}

		~VulkanTestLayer() {}

		/// 
		/// Main
		/// 

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		void OnAttach() override;

		void OnDetach() override;


		void BuildTestCommandBuffer();

	private:

		Ref<Texture2D> m_Tetxure1;
		Ref<Texture2D> m_Tetxure2;

		GraphicsPipeline m_GraphicsPipeline;
		Ref<Framebuffer> m_FrameBuffer;

		Ref<EditorCameraController> m_EditorCamera = nullptr;

		glm::vec3 m_AddColor = glm::vec3(1);
	};
}