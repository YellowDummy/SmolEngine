#pragma once
#include <memory>
#include <functional>

#include "Core.h"
#include "EventHandler.h"
#include "LayerManager.h"
#include "SLog.h"
#include "Core/Time.h"

namespace SmolEngine
{
	class ImGuiLayer;

	class Window;

	class UIText;

	struct DeltaTime;

	///

	class Application
	{
	public:

		Application();

		Application(const Application&) = delete;

		virtual ~Application();

		/// Main

		void InitApp();

		void CloseApp();

		void RunApp();

		void OnEvent(Event& event);

		/// Overrides

		virtual void ClientInit() = 0;

		/// Layers

		void PushLayer(Layer* layer);

		void PushOverlay(Layer* layer);

		void PopLayer();

		/// Scripting

		void InitializeScripts();

		///Callbacks

		bool OnWindowClose(Event& e);

		bool OnWindowResize(Event& e);

		/// Getters

		Window& GetWindow() { return *m_Window; }

		static Application& GetApplication() { return *s_Instance; }

		const uint32_t GetWindowHeight();

		const uint32_t GetWindowWidth();

	protected:

		ImGuiLayer* m_ImGuiLayer;

		///

		Ref<Window> m_Window = nullptr;

		Ref<LayerManager> m_LayerHandler = nullptr;

		Ref<EventHandler> m_EventHandler = nullptr;

		UIText* m_UIText = nullptr;

	private:

		///

		static Application* s_Instance;

		bool m_Running, m_WindowMinimized;

		float m_LastFrameTime;
	};

	Application* CreateApp(); 	// Client side 

}

