#pragma once

#include "Common/Core.h"

#include <string>

struct GLFWwindow;

namespace Frostium 
{
	class EventSender;

	struct WindowData
	{
		uint32_t            Width;
		uint32_t            Height;
		std::string         Title;
	};

	struct WindowCreateInfo
	{
		uint32_t            Width = 720;
		uint32_t            Height = 480;
		bool                bVSync = false;
		bool                bFullscreen = false;
		std::string         Title = "SomeApplication";
	};

	class Window
	{
	public:

		void Init(const WindowCreateInfo* info, EventSender* eventHandler);
		void ProcessEvents();
		void ShutDown();

		// Getters
		GLFWwindow* GetNativeWindow() const;
		WindowData* GetWindowData();
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		// Setters
		void SetWidth(uint32_t value);
		void SetHeight(uint32_t value);
		void SetVSync(bool enabled);
		void SetTitle(const std::string& name);

	private:

		void Create(const WindowCreateInfo* info);

	private:

		GLFWwindow*     m_Window = nullptr;
		WindowData      m_Data = {};
	};

}
