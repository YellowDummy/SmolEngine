#pragma once

#include "Renderer/GraphicsContext.h"
#include "Renderer/OpenGL/OpenglContext.h"
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

		std::string         Title;
		uint32_t            Width, Height;
		Ref<EventHandler>   m_EventHandler;
	};

	struct WindowCreateInfo
	{
		uint32_t            Width = 0;
		uint32_t            Height = 0;
		Ref<EventHandler>   EventHandler = nullptr;
		std::string         Title = "";
	};

	static WindowData Data;

	class Window
	{
	public:

		Window(const WindowCreateInfo& info);

		~Window() {  }

		/// Main

		void ResizeContext(uint32_t width, uint32_t height);

		void ProcessEvents();

		void SwapBuffers();

		void BeginFrame();

		void ShutDown();

		/// Getters

		GLFWwindow* GetNativeWindow() const;

		GraphicsContext* GetContext() const;

		uint32_t GetWidth() const;

		uint32_t GetHeight() const;

		/// Setters

		void SetWidth(uint32_t value);

		void SetHeight(uint32_t value);

	public:

		GLFWwindow* m_Window;

	private:

		GraphicsContext* m_Context;

		void WidnowInit(const std::string& title, const int& height, const int& width);

		void SetVSync(bool enabled);
	};

}
