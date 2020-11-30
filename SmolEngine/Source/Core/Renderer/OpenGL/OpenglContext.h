#pragma once

struct GLFWwindow;

namespace SmolEngine 
{
	class OpenglContext
	{
	public:

		/// 
		/// Main
		/// 

		void Setup(GLFWwindow* window);

		void SwapBuffers();

		/// 
		/// Getters
		/// 

		inline GLFWwindow* GetWindow() { return m_Window; }

	private:

		GLFWwindow* m_Window =  nullptr;
	};
}

