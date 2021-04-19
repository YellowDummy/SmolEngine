#pragma once
#include "Common/Core.h"
#include "Common/Window.h"

#include <functional>

namespace Frostium
{
	enum class EventType : uint32_t
	{
		EVENT_TYPE_NONE = 0,

		KEY_PRESS,
		KEY_RELEASE,
		KEY_REPEAT,

		MOUSE_MOVE,
		MOUSE_BUTTON,
		MOUSE_SCROLL,
		MOUSE_PRESS,
		MOUSE_RELEASE,

		WINDOW_CLOSE,
		WINDOW_RESIZE,
		WINDOW_UPDATE,
	};

	enum class EventCategory : uint32_t
	{
		EVENT_NONE = 0,
		EVENT_KEYBOARD,
		EVENT_MOUSE,
		EVENT_APP
	};

	class Event
	{
	public:

		Event() = default;
		virtual ~Event() = default;

		bool IsType(EventType type);
		bool IsCategory(EventCategory category);
		static bool IsEventReceived(Frostium::EventType type, Event& event);

		template<typename T>
		T* Cast()
		{
			return static_cast<T*>(this);
		}

	public:

		bool            m_Handled = false;
		EventType       m_EventType = EventType::EVENT_TYPE_NONE;
		EventCategory   m_EventCategory = EventCategory::EVENT_NONE;
		uint32_t        m_Action = 0;
		uint32_t        m_Key = 0;
	};

	class EventSender
	{
	public:

		void SendEvent(Event& event, EventType eventType, EventCategory eventCategory, int action = -1, int key = -1);

	public:

		std::function<void(Event& event_to_send)> OnEventFn;
	};

	/*
	   Events
	*/

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(WindowData& data) :
			m_Data(data) {}

		inline const unsigned int GetWidth() { return m_Data.Width; }
		inline const unsigned int GetHeight() { return m_Data.Height; }

	private:
		WindowData& m_Data;
	};

	class WindowCloseEvent : public Event
	{
	public:

		WindowCloseEvent() = default;
	};

	class KeyEvent : public Event
	{
	public:

		KeyEvent() = default;
	};

	class MouseMoveEvent : public Event
	{
	public:

		MouseMoveEvent(double& xPos, double& yPos)
			:m_xPos(xPos), m_yPos(yPos) {}

		double m_xPos, m_yPos;
	};

	class MouseScrollEvent : public Event
	{
	public:

		MouseScrollEvent(float xOffset, float yOffset)
			:m_xOffset(xOffset), m_yOffset(yOffset) {}

		inline const float GetXoffset() { return m_xOffset; }
		inline const float GetYoffset() { return m_yOffset; }

	private:
		float m_xOffset, m_yOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:

		MouseButtonEvent(int mouseButton)
			:m_MouseButton(m_MouseButton) {}

		int m_MouseButton;
	};
}