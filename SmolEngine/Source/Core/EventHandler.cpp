#include "stdafx.h"
#include "SLog.h"
#include "EventHandler.h"
#include "Events/InputEvent.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

namespace SmolEngine 
{
	void EventHandler::SendEvent(Event& event, EventType eventType, EventCategory eventCategory, int action, int key)
	{
		event.m_EventType = (int)eventType; event.m_EventCategory = (int)eventCategory; event.m_Key = key; event.m_Action = action; event.m_Handled = false;

		OnEventFn(event);
	}

	EventHandler::EventHandler()
	{

	}

	EventHandler::~EventHandler()
	{

	}



}
