#pragma once
#include "Core.h"
#include <string>

#include <Frostium3D/Common/Time.h>
#include <Frostium3D/Common/Events.h>


namespace SmolEngine 
{
	class Layer
	{
	public:

		Layer(const std::string& name = "Default Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnBeginFrame(Frostium::DeltaTime deltaTime) {};
		virtual void OnEndFrame(Frostium::DeltaTime deltaTime) {};
		virtual void OnUpdate(Frostium::DeltaTime deltaTime) {}
		virtual void OnEvent(Frostium::Event& event) {}
		virtual void OnImGuiRender() {}

		inline const std::string& GetName() const { return m_Name; }

	protected:

		std::string   m_Name;
		bool          m_Enabled = true;
	};
}

