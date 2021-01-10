#pragma once

#include "Core/Layer.h"
#include "Core/Core.h"

#ifndef SMOLENIGNE_OPENGL_IMPL
#include "ImGui/ImGuiVulkanImpl.h"
#endif

namespace SmolEngine 
{
	class Window;

	class ImGuiLayer: public Layer
	{
	public:

		ImGuiLayer();

		~ImGuiLayer();

		void OnEvent(Event& event) override;

		void OnAttach() override;

		void OnDetach() override;

		void OnImGuiRender() override;

		void OnBegin();

		void OnEnd();

	private:

#ifndef SMOLENGINE_OPENGL_IMPL

		ImGuiVulkanImpl m_VulkanImpl = {};
#endif
	};

}
