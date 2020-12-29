#pragma once

#include "Core/ECS/ComponentTuples/BaseTuple.h"
#include "Core/ECS/Components/Singletons/AudioEngineSComponent.h"
#include "Core/ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "Core/ECS/Components/Singletons/FramebufferSComponent.h"
#include "Core/ECS/Components/Singletons/ProjectConfigSComponent.h"

namespace SmolEngine
{
	struct SingletonTuple: public BaseTuple
	{
		SingletonTuple();

		/// Data

		AudioEngineSComponent AudioEngine;
		Box2DWorldSComponent Box2DWord;
		FramebufferSComponent Framebuffers;

	private:

		friend class EditorLayer;
		friend class Scene;
	};
}