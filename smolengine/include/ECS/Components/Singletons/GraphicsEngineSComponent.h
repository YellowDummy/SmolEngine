#pragma once

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include "Frostium3D/GraphicsContext.h"
#include "Frostium3D/Common/RendererStorage.h"
#include "Frostium3D/Common/Renderer2DStorage.h"

namespace SmolEngine
{
	struct GraphicsEngineSComponent
	{
		GraphicsEngineSComponent();
		~GraphicsEngineSComponent();
		// Dummy c-tors - required by EnTT
		GraphicsEngineSComponent(GraphicsEngineSComponent& another) {}
		GraphicsEngineSComponent(GraphicsEngineSComponent&& other) {}
		GraphicsEngineSComponent& operator=(GraphicsEngineSComponent other) { return *this; }
		
		RendererStorage   Strorage{};
		Renderer2DStorage Storage2D{};

		static GraphicsEngineSComponent* Get() { return Instance; }

	private:

		inline static GraphicsEngineSComponent* Instance = nullptr;
	};
}