#pragma once
#include "Core.h"
#include <string>
#include "Core/Time.h"
#include "EventHandler.h"


namespace SmolEngine 
{

	class Layer
	{
	public:

		Layer(const std::string& name = "Basic Layer");

		virtual ~Layer();

		virtual void OnAttach() {}

		virtual void OnDetach() {}

		virtual void OnUpdate(DeltaTime deltaTime) {}

		virtual void OnEvent(Event& event) {}

		virtual void OnImGuiRender() {}

		inline const std::string& GetName() const { return m_Name; }

	protected:

		std::string m_Name;
		bool enabled = true;
	};
}

