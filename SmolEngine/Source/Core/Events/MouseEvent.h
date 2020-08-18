#pragma once
#include "Core/SLog.h"
#include "Core/EventHandler.h"

namespace SmolEngine
{
	class MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(double& xPos, double& yPos)
			:m_xPos(xPos), m_yPos(yPos)
		{

		}


		double m_xPos, m_yPos;
	};

	class MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float xOffset, float yOffset)
			:m_xOffset(xOffset), m_yOffset(yOffset)
		{}

		inline const float GetXoffset() { return m_xOffset; }
		inline const float GetYoffset() { return m_yOffset; }

	private:
		float m_xOffset, m_yOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(int mouseButton) 
			:m_MouseButton(m_MouseButton)
		{}

		int m_MouseButton;
	};
}