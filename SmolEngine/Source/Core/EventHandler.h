#pragma once
#include <map>
#include <functional>


namespace SmolEngine 
{
	class Application;

	enum class EventType: uint32_t
	{

		S_KEY_PRESS, S_KEY_RELEASE, S_KEY_REPEAT,
		S_MOUSE_MOVE, S_MOUSE_BUTTON, S_MOUSE_SCROLL, S_MOUSE_PRESS, S_MOUSE_RELEASE,
		S_WINDOW_CLOSE, S_WINDOW_RESIZE, S_WINDOW_UPDATE,
	};

	enum class EventCategory: uint32_t
	{
		S_EVENT_NONE = 0,
		S_EVENT_KEYBOARD, S_EVENT_MOUSE, S_EVENT_APPLICATION,
	};

	class Event
	{
	public:

		Event() {}

		virtual ~Event() {}

	public:

		bool m_Handled = false;
		uint32_t m_EventType = -1, m_EventCategory = -1, m_Action = -1, m_Key = -1;
	};

	class EventHandler
	{
	public:

		EventHandler();

		~EventHandler();

		void SendEvent(Event& event, EventType eventType, EventCategory eventCategory, int action = -1, int key = -1);

		std::function<void(Event& event_to_send)> OnEventFn;
	};
}

