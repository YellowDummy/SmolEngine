#pragma once
#include "Core.h"

namespace SmolEngine
{
	class SMOL_ENGINE_API Application
	{
	public:

		Application();
		virtual ~Application();
		void EngineInit();
		void StartApp();
	};

	//client side 
	Application* CreateApp();

}

