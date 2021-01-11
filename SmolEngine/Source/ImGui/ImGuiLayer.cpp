#include "stdafx.h"
#include "ImGuiLayer.h"

#define GLFW_INCLUDE_VULKAN
#define IMGUI_VULKAN_DEBUG_REPORT

#include <../Libraries/imgui/imgui.h>
#include "../Libraries/imgui/imgui_internal.h"
#include <../Libraries/imgui/examples/imgui_impl_glfw.h>
#include <../Libraries/imgui/examples/imgui_impl_opengl3.h>
#include <../Libraries/imgui/examples/imgui_impl_vulkan.h>

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "Core/Window.h"
#include "Core/EventHandler.h"
#include "Core/Application.h"
#include "IconsFontAwesome5.h"
#include "ImGuizmo.h"
#include "Core/Window.h"

#include "NodeEditor/imnodes.h"

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
		ImGuiIO& io = ImGui::GetIO();

		if (io.WantTextInput && event.m_EventCategory == (uint32_t)EventCategory::S_EVENT_KEYBOARD)
		{
			event.m_Handled = true;
		}
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		imnodes::Initialize();

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
			style.ChildRounding = 3.f;
			style.GrabRounding = 0.f;
			style.WindowRounding = 0.f;
			style.ScrollbarRounding = 3.f;
			style.FrameRounding = 3.f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);


			style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			//style.Colors[ImGuiCol_ComboBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);

			//style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			//style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			//style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
			style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);

			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		}

		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 1;
		config.GlyphExtraSpacing.x = 1.0f;

#ifdef SMOLENGINE_EDITOR

		io.Fonts->AddFontFromFileTTF("../Resources/Fonts/Font1.ttf", 17.0f, &config);
#else
		io.Fonts->AddFontFromFileTTF("../../Resources/Fonts/Font1.ttf", 17.0f, &config);
#endif

		Application& app = Application::GetApplication();
		GLFWwindow* EditorWindow = app.GetWindow().GetNativeWindow();

#ifdef  SMOLENGINE_OPENGL_IMPL

		ImGui_ImplGlfw_InitForOpenGL(EditorWindow, true);

		ImGui_ImplOpenGL3_Init("#version 410");
#else
		m_VulkanImpl.Init();

		ImGui_ImplGlfw_InitForVulkan(EditorWindow, true);

		ImGui_ImplVulkan_InitInfo init_info = {};
		{
			init_info.Instance = VulkanContext::GetInstance().GetInstance();
			init_info.PhysicalDevice = VulkanContext::GetDevice().GetPhysicalDevice();
			init_info.Device = VulkanContext::GetDevice().GetLogicalDevice();
			init_info.QueueFamily = VulkanContext::GetDevice().GetQueueFamilyIndex();
			init_info.Queue = VulkanContext::GetDevice().GetQueue();
			init_info.DescriptorPool = m_VulkanImpl.g_DescriptorPool;
			init_info.PipelineCache = m_VulkanImpl.g_PipelineCache;
			init_info.Allocator = nullptr;
			init_info.MinImageCount = 2;
			init_info.ImageCount = 3; // temp
		}

		ImGui_ImplVulkan_Init(&init_info, VulkanContext::GetSwapchain().GetRenderPass());

		m_VulkanImpl.InitResources();

#endif //  SMOLENGINE_OPENGL_IMPL
	}

	void ImGuiLayer::OnDetach()
	{
#ifdef  SMOLENGINE_OPENGL_IMPL

		ImGui_ImplOpenGL3_Shutdown();
#else
		ImGui_ImplVulkan_Shutdown();
#endif

		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		imnodes::Shutdown();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::OnBegin()
	{
#ifdef  SMOLENGINE_OPENGL_IMPL

		ImGui_ImplOpenGL3_NewFrame();
#else
		ImGui_ImplVulkan_NewFrame();
#endif
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}


	void ImGuiLayer::OnEnd()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)Data.Width, (float)Data.Height);

		ImGui::Render();

#ifdef  SMOLENGINE_OPENGL_IMPL

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else

#endif

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}