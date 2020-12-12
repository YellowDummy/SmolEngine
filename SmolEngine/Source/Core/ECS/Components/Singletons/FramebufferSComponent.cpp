#include "stdafx.h"
#include "FramebufferSComponent.h"

#include "Core/Application.h"

namespace SmolEngine
{
	FramebufferSComponent* FramebufferSComponent::Instance = nullptr;

	FramebufferSComponent::FramebufferSComponent()
	{
		FramebufferSpecification spec;
		spec.Width = Application::GetApplication().GetWindowWidth();
		spec.Height = Application::GetApplication().GetWindowHeight();
		auto frameBuffer = Framebuffer::Create(spec);

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