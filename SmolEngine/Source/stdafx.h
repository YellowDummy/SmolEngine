#pragma once

#ifdef PLATFORM_WIN
#include <Windows.h>
#endif

#include <memory>
#include <iostream>
#include <algorithm>
#include <functional>
#include <thread>
#include <utility>
#include <fstream>
#include <sstream>
#include <filesystem>


#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <string>
#include <stack>


#include "../../SmolEngine/Source/Core/ECS/Scene.h"
#include "../../SmolEngine/Source/Core/ECS/Components.h"
#include "../../SmolEngine/Source/Core/ECS/Actor.h"
#include "../../SmolEngine/Source/Core/ECS/ScriptableObject.h"

#include "../../SmolEngine/Source/Core/Audio/AudioEngine.h"
#include "../../SmolEngine/Source/Core/UI/UICanvas.h"

#include "../../SmolEngine/Source/Core/Renderer/Renderer2D.h"
#include "../../SmolEngine/Source/Core/Renderer/RendererAPI.h"
#include "../../SmolEngine/Source/Core/Renderer/Renderer.h"
#include "../../SmolEngine/Source/Core/Renderer/Framebuffer.h"
#include "../../SmolEngine/Source/Core/Renderer/Camera.h"
#include "../../SmolEngine/Source/Core/Renderer/Shader.h"
#include "../../SmolEngine/Source/Core/Renderer/SubTexture.h"
#include "../../SmolEngine/Source/Core/Renderer/Texture.h"
#include "../../SmolEngine/Source/Core/Renderer/Buffer.h"

#include "../../SmolEngine/Source/Core/Renderer/OpenGL/OpenglRendererAPI.h"
#include "../../SmolEngine/Source/Core/Renderer/OpenGL/OpenglShader.h"
#include "../../SmolEngine/Source/Core/Renderer/OpenGL/OpenglTexture.h"

#include "../../SmolEngine/Source/Core/Application.h"

#include "../../SmolEngine/Source/Core/ImGui/AnimationPanel.h"
#include "../../SmolEngine/Source/Core/ImGui/BuildPanel.h"
#include "../../SmolEngine/Source/Core/ImGui/EditorConsole.h"
#include "../../SmolEngine/Source/Core/ImGui/ImGuiLayer.h"
#include "../../SmolEngine/Source/Core/ImGui/FileBrowser/imfilebrowser.h"

#include <../../SmolEngine/Libraries/cereal/include/cereal/cereal.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/types/polymorphic.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/archives/json.hpp>
#include <../../SmolEngine/Libraries/glm/glm/glm.hpp>


