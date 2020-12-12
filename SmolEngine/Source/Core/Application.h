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


	class Application
	{
	public:

		Application(const Application&) = delete;

		virtual ~Application();

		Application();


		/// 
		/// Main
		/// 

		void OnEvent(Event& event);

		void CloseApp();

		void InitApp();

		void RunApp();

		/// 
		/// Overrides
		/// 

		virtual void ClientInit() = 0;

		/// 
		/// Layers
		/// 

		void PushOverlay(Layer* layer);

		void PushLayer(Layer* layer);

		void PopLayer();

		/// 
		/// Callbacks
		/// 

		bool OnWindowResize(Event& e);

		bool OnWindowClose(Event& e);

		/// 
		/// Getters
		/// 

		static Application& GetApplication() { return *s_Instance; }

		Window& GetWindow() { return *m_Window; }

		const uint32_t GetWindowHeight();

		const uint32_t GetWindowWidth();

	protected:

		ImGuiLayer* m_ImGuiLayer;


		Ref<LayerManager> m_LayerHandler = nullptr;

		Ref<EventHandler> m_EventHandler = nullptr;

		Ref<Window> m_Window = nullptr;


		UIText* m_UIText = nullptr;

	private:

		static Application* s_Instance;

		bool m_Running, m_WindowMinimized;

		float m_LastFrameTime;
	};

	Application* CreateApp(); 	// Client side 

}

