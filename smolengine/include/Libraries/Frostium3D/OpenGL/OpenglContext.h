#pragma once
#ifdef FROSTIUM_OPENGL_IMPL
struct GLFWwindow;

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class OpenglContext
	{
	public:

		void Setup(GLFWwindow* window);
		void SwapBuffers();

		// Getters
		inline GLFWwindow* GetWindow() { return m_Window; }

	private:

		GLFWwindow* m_Window = nullptr;
	};
}


#endif