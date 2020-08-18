#pragma once

namespace SmolEngine 
{
	class GraphicsContext
	{
	public:
		virtual void Setup() = 0;
		virtual void SwapBuffers() = 0;
	};
}
