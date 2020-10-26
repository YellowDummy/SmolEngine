#include "stdafx.h"
#include "FramebufferSComponent.h"

#include "Core/Application.h"

namespace SmolEngine
{
	FramebufferSComponent* FramebufferSComponent::Instance = nullptr;

	FramebufferSComponent::FramebufferSComponent()
	{
		FramebufferData m_FramebufferData;
		m_FramebufferData.Width = Application::GetApplication().GetWindowWidth();
		m_FramebufferData.Height = Application::GetApplication().GetWindowHeight();
		auto frameBuffer = Framebuffer::Create(m_FramebufferData);

		Framebuffers[0] = frameBuffer;
		Instance = this;
	}

	FramebufferSComponent::~FramebufferSComponent()
	{
		if (!Instance) { return; }

		Framebuffers.clear();

		Instance = nullptr;
	}
}