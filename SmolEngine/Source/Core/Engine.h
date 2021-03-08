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
	class WorldAdmin;

	class Engine
	{
	public:

		virtual ~Engine();

		Engine();

		/// Main

		void Init();

		void Shutdown();

		void Run();

		/// Layers

		void PushOverlay(Layer* layer);

		void PushLayer(Layer* layer);

		void PopLayer();

		/// Callbacks

		void OnWindowResize(Event& e);

		void OnWindowClose(Event& e);

		void OnEvent(Event& event);

		/// Overrides

		virtual void OnEngineInitialized() {};

		/// Getters

		static Engine& GetEngine() { return *s_Instance; }

		void GetAppName(std::string& outName);

		Window& GetWindow() { return *m_Window; }

		const uint32_t GetWindowHeight();

		const uint32_t GetWindowWidth();

	private:

		ImGuiLayer*            m_ImGuiLayer = nullptr;
		UIText*                m_UIText = nullptr;
		WorldAdmin*            m_World = nullptr;
		Ref<LayerManager>      m_LayerHandler = nullptr;
		Ref<EventHandler>      m_EventHandler = nullptr;
		Ref<Window>            m_Window = nullptr;

		bool                   m_Running = false;
		bool                   m_WindowMinimized = false;
		float                  m_LastFrameTime = 0.0f;

		static Engine*    s_Instance;
	};

	Engine* CreateEngineContext();

}

