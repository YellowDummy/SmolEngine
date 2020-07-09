#pragma once

#include "SLog.h"
#include "Core.h"
#include <GLFW\glfw3.h>
#include <glad/glad.h>

namespace SmolEngine 
{
	struct WindowData
	{
		std::string TitleName;
		unsigned int Width;
		unsigned int Height;

		WindowData(const std::string& tilteName = "SmolEngine", unsigned int width = 1280, unsigned int height = 720)
			: TitleName(tilteName), Width(width), Height(height)
		{

		}
	};

	class SMOL_ENGINE_API Window 
	{
	public:
		GLFWwindow* m_Window;

		Window(const std::string& title, const int& height, const int& width)
			:m_Window(nullptr)
		{
			WidnowInit(title,height, width);
		}

		virtual ~Window() { glfwDestroyWindow(m_Window); }

		void OnUpdate();
		inline unsigned int GetWidth() const;
		inline unsigned int GetHeight() const;

	private:

		static void GLFWErrorCallback(int error, const char* description) 
		{
			NATIVE_ERROR("GLFW Error ({0}): {1}", error, description);
		}

		void WidnowInit(const std::string& title, const int& height, const int& width)
		{
			glfwSetErrorCallback(GLFWErrorCallback);
			glfwInit();

			Data.Title = title;
			Data.Height = height;
			Data.Width = width;

			m_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), NULL, NULL);
			if (!m_Window)
			{
				return;
			}

			NATIVE_INFO("Created window: {0}, ({1}, {2})", title.c_str(), height, width);

			gladLoadGL();
			glfwMakeContextCurrent(m_Window);
			SetVSync(true);
		}

		void ShutDown()
		{
			NATIVE_INFO("Closing window...");
			glfwDestroyWindow(m_Window);
		}

		void SetVSync(bool enabled)
		{
			NATIVE_INFO("VSync state: {0}", enabled);
			enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
		}

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
		};

		WindowData Data;

	};
}
