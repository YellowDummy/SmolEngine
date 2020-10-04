#pragma once

#include "Core/Layer.h"
#include "Core/Core.h"


namespace SmolEngine 
{
	class Window;

	class ImGuiLayer: public Layer
	{
	public:

		ImGuiLayer();

		~ImGuiLayer();

		void OnEvent(Event& event) override;

		void OnAttach() override;

		void OnDetach() override;

		void OnImGuiRender() override;

		void OnBegin();

		void OnEnd();

	private:
	};

}
