#include "stdafx.h"
#include "ImGuiLayer.h"

#include <../Libraries/imgui/imgui.h>
#include <../Libraries/imgui/examples/imgui_impl_glfw.h>
#include <../Libraries/imgui/examples/imgui_impl_opengl3.h>

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "Core/Window.h"
#include "Core/EventHandler.h"
#include "Core/Application.h"

namespace SmolEngine 
{

	ImGuiLayer::ImGuiLayer()
		:Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		//TO DO: Block Mouse & Keyboard events
		//ImGuiIO& io = ImGui::GetIO();
		//event.m_Handled |= event.m_EventCategory == (uint32_t) EventType::S_MOUSE_MOVE && io.WantCaptureMouse;
		//event.m_Handled |= event.m_EventCategory == (uint32_t)EventCategory::S_EVENT_KEYBOARD && io.WantCaptureKeyboard;
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::GetApplication();
		GLFWwindow* window = (app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::OnBegin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnEnd()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::GetApplication();
		io.DisplaySize = ImVec2((float)Data.Width, (float)Data.Height);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}
