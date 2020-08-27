#pragma once
#include "Core/Layer.h"


namespace SmolEngine 
{
	class SMOL_ENGINE_API ImGuiLayer: public Layer
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
