#pragma once
#include "../SLog.h"
#include "Core/EventHandler.h"
#include "Core/SLog.h"
#include "Core/Window.h"

namespace SmolEngine 
{

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(WindowData& data):
			m_Data(data)
		{ 

		}

		inline const unsigned int GetWidth() { return m_Data.Width; }
		inline const unsigned int GetHeight() { return m_Data.Height; }

	private:
		WindowData& m_Data;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent()
		{

		}
	};
}
