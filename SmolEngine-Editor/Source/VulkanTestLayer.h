#pragma once

#include "SmolEngineCore.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Vulkan/VulkanPipeline.h"
#include "Core/Renderer/Vulkan/VulkanIndexBuffer.h"
#include "Core/Renderer/Vulkan/VulkanVertexBuffer.h"

#include "Core/Renderer/Vulkan/VulkanTexture.h"

#include "Core/Renderer/Camera.h"

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

		VulkanPipeline m_Pipeline = {};
		VulkanTexture m_TestTexture = {};

		VulkanIndexBuffer m_IndexBuffer = {};
		VulkanVertexBuffer m_VertexBuffer = {};

		Ref<Shader> m_Shader = nullptr;

		Ref<EditorCameraController> m_EditorCamera = nullptr;

		glm::vec3 m_AddColor = glm::vec3(1);
	};
}