#pragma once

#include "ECS/ComponentTuples/BaseTuple.h"
#include "ECS/Components/Singletons/AudioEngineSComponent.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "ECS/Components/Singletons/FramebufferSComponent.h"
#include "ECS/Components/Singletons/ProjectConfigSComponent.h"

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