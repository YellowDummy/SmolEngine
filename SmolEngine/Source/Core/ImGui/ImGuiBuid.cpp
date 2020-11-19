#include "stdafx.h"


#ifdef SMOLENGINE_OPENGL_IMPL

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <../Libraries/imgui/examples/imgui_impl_glfw.cpp>
#include <../Libraries/imgui/examples/imgui_impl_opengl3.cpp>

#else

#include <../Libraries/imgui/examples/imgui_impl_glfw.cpp>
#include <../Libraries/imgui/examples/imgui_impl_vulkan.h>

#endif // SMOLENGINE_OPENGL_IMPL

#include <../../imgui/misc/cpp/imgui_stdlib.cpp>