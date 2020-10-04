#pragma once

#include "Core/Renderer/GraphicsContext.h"
#include "Core/Renderer/OpenGL/OpenglContext.h"
#include "Core/Core.h"
#include "SLog.h"
#include "EventHandler.h"

namespace SmolEngine 
{
	struct WindowData
	{
		WindowData()
			:Title("SmolEngine v0.1"), Width(720), Height(480)
		{

		}

		std::string Title;

		unsigned int Width, Height;

		Ref<EventHandler> m_eventHandler;
	};

	static WindowData Data;

	class Window
	{
	public:

		GLFWwindow* m_Window;

		Window(const std::string& title, const int& height, const int& width, Ref<EventHandler> eventHandler);

		virtual ~Window() {  }

		void OnUpdate();

		void SetWidth(int);

		void SetHeight(int);

		unsigned int GetWidth() const;

		unsigned int GetHeight() const;

		GLFWwindow* GetNativeWindow() const { return m_Window; }

		void ShutDown();

	private:

		GraphicsContext* m_Context;

		void WidnowInit(const std::string& title, const int& height, const int& width);

		void SetVSync(bool enabled);
	};

}
