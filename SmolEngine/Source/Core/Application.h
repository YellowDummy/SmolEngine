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
	struct DeltaTime;


	class Application
	{
	public:

		Application();
		Application(const Application&) = delete;
		virtual ~Application();

		void InitApp();
		void CloseApp();
		void BindAction(const int eventType, const int key, const void *event_ptr);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer();

		//Callbacks
		bool OnWindowClose(Event& e);
		bool OnWindowResize(Event& e);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& GetApplication() { return *s_Instance; }

		const uint32_t GetWindowHeight();
		const uint32_t GetWindowWidth();

	protected:
		ImGuiLayer* m_ImGuiLayer;
		Ref<Window> m_Window;
		Ref<LayerManager> m_LayerHandler;
		Ref<EventHandler> m_EventHandler;

	private:
		virtual void ClientInit() = 0;
		void RunApp();
		void OnEvent(Event& event);

	private:
		static Application* s_Instance;
		bool m_Running, m_WindowMinimized;
		float m_LastFrameTime;
	};

	//client side 
	Application* CreateApp();

}

