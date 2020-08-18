#include "stdafx.h"
#include "SLog.h"
#include "EventHandler.h"
#include "Core/Events/InputEvent.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"

namespace SmolEngine 
{
	void EventHandler::SendEvent(Event& event, EventType eventType, EventCategory eventCategory, int action, int key)
	{
		event.m_EventType = (int)eventType; event.m_EventCategory = (int)eventCategory; event.m_Key = key; event.m_Action = action; event.m_Handled = false;

		if (eventCategory == EventCategory::S_EVENT_KEYBOARD && eventType == EventType::S_KEY_REPEAT)
		{
			auto value = m_InputMap[(int)eventType + key];
			if (value)
			{
				void* action = (void*)value;
				if (action)
				{
					((void (*)(void))action)();
				}
			}

		}

		OnEventFn(event);
	}

	void EventHandler::BindAction(const int eventType, const int key, const void(*event_ptr))
	{
		m_InputMap[eventType + key] = event_ptr; return;
	}

	EventHandler::EventHandler()
	{

	}

	EventHandler::~EventHandler()
	{

	}



}
