#pragma once

#ifndef FROSTIUM_OPENGL_IMPL
#include <ImGUI/ImGuiVulkanImpl.h>
#endif
#include "Common/Events.h"

namespace Frostium 
{
	struct GraphicsContextInitInfo;

	class Window;
	class ImGuiContext
	{
	public:

		void Init(GraphicsContextInitInfo* info);
		void ShutDown();

		// Events
		void OnEvent(Event& event);
		void OnBegin();
		void OnEnd();

	private:

#ifndef FROSTIUM_OPENGL_IMPL
		ImGuiVulkanImpl m_VulkanImpl = {};
#endif
	};

}
