#pragma once
#include "Core.h"
#include "Core/SLog.h"
#include "Core/Time.h"
#include "Core/EventHandler.h"
#include "Core/LayerManager.h"

#include <memory>
#include <functional>

namespace SmolEngine
{
	struct DeltaTime;

	class ImGuiLayer;
	class Window;
	class UIText;

	class Application
	{
	public:

		Application(const Application&) = delete;

		virtual ~Application();

		Application();

		/// Main

		void OnEvent(Event& event);

		void CloseApp();

		void InitApp();

		void RunApp();

		void InitGraphicsContext();

		/// Layers

		void PushOverlay(Layer* layer);

		void PushLayer(Layer* layer);

		void PopLayer();

		/// Callbacks

		bool OnWindowResize(Event& e);

		bool OnWindowClose(Event& e);

		/// Overrides

		virtual void ClientInit() = 0;

		/// Getters

		static Application& GetApplication() { return *s_Instance; }

		void GetAppName(std::string& outName);

		Window& GetWindow() { return *m_Window; }

		const uint32_t GetWindowHeight();

		const uint32_t GetWindowWidth();

	private:

		ImGuiLayer*            m_ImGuiLayer = nullptr;
		UIText*                m_UIText = nullptr;
		Ref<LayerManager>      m_LayerHandler = nullptr;
		Ref<EventHandler>      m_EventHandler = nullptr;
		Ref<Window>            m_Window = nullptr;

		bool                   m_Running = false;
		bool                   m_WindowMinimized = false;
		float                  m_LastFrameTime = 0.0f;

		static Application*    s_Instance;
	};

	Application* CreateApp(); 	// Client side 

}

