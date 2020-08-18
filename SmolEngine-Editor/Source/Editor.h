#pragma once

#include "SmolEngineCore.h"

namespace SmolEngine
{
	class Editor : public Application 
	{
	public:
		Editor();
		~Editor();

		void ClientInit() override;
	};

}

