#pragma once
#include "SmolEngineCore.h"

namespace SmolEngine
{
	class RaytracingTestLayer : public Layer
	{
	public:

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		void OnAttach() override;

		void OnDetach() override;
	};
}