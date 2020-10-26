#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Framebuffer.h"

#include <unordered_map>

namespace SmolEngine
{
	// Note:
	// S - Singleton Component
	// Contains all framebuffers for active scene
	// Default framebuffer initialized with index 0

	struct FramebufferSComponent
	{
		FramebufferSComponent();

		~FramebufferSComponent();

		/// Data

		std::unordered_map<size_t, Ref<Framebuffer>> Framebuffers;

		/// Getters

		static std::unordered_map<size_t, Ref<Framebuffer>>& Get() { return Instance->Framebuffers; }

	private:

		static FramebufferSComponent* Instance;
	};
}