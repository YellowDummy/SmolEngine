#pragma once

#include "SmolEngineCore.h"

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
	};
}