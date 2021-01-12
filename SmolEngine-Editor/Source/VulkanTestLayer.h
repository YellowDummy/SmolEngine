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
	class VulkanTestLayer: public Layer
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

	};
}