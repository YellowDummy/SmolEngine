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
	class UIText;


	class Application
	{
	public:

		Application();

		Application(const Application&) = delete;

		virtual ~Application();

		void InitApp();

		void CloseApp();

		void PushLayer(Layer* layer);

		void PushOverlay(Layer* layer);

		void PopLayer();

		void InitializeScripts();

		//Callbacks
		bool OnWindowClose(Event& e);

		bool OnWindowResize(Event& e);

		Window& GetWindow() { return *m_Window; }

		static Application& GetApplication() { return *s_Instance; }

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

		UIText* m_UIText = nullptr;

		static Application* s_Instance;

		bool m_Running, m_WindowMinimized;

		float m_LastFrameTime;
	};

	//client side 
	Application* CreateApp();

}

