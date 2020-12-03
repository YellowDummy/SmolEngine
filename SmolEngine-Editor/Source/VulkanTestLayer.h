#pragma once

#include "SmolEngineCore.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Vulkan/VulkanPipeline.h"

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

		Ref<Shader> m_Shader = nullptr;
	};
}