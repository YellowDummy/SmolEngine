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


#include "../../SmolEngine/Source/ECS/WorldAdmin.h"
#include "../../SmolEngine/Source/ECS/Actor.h"

#include "../../SmolEngine/Source/Audio/AudioEngine.h"

#include "../../SmolEngine/Source/Renderer/Renderer2D.h"
#include "../../SmolEngine/Source/Renderer/RendererAPI.h"
#include "../../SmolEngine/Source/Renderer/Renderer.h"
#include "../../SmolEngine/Source/Renderer/Framebuffer.h"
#include "../../SmolEngine/Source/Renderer/Camera.h"
#include "../../SmolEngine/Source/Renderer/Shader.h"
#include "../../SmolEngine/Source/Renderer/SubTexture.h"
#include "../../SmolEngine/Source/Renderer/Texture.h"
#include "../../SmolEngine/Source/Renderer/Buffer.h"

#include "../../SmolEngine/Source/Renderer/OpenGL/OpenglRendererAPI.h"
#include "../../SmolEngine/Source/Renderer/OpenGL/OpenglShader.h"
#include "../../SmolEngine/Source/Renderer/OpenGL/OpenglTexture.h"

#ifndef SMOLENGINE_OPENGL_IMPL
#include "../../SmolEngine/Source/Renderer/Vulkan/Vulkan.h"
#include "../../SmolEngine/Source/Renderer/Vulkan/VulkanContext.h"
#endif

#include "ECS/Components/BehaviourComponent.h"
#include "ECS/Components/Light2DSourceComponent.h"

#include "ECS/ComponentTuples/DefaultBaseTuple.h"
#include "ECS/ComponentTuples/CameraBaseTuple.h"
#include "ECS/ComponentTuples/SingletonTuple.h"

#include "../../SmolEngine/Source/Core/Application.h"

#include "../../SmolEngine/Source/ImGui/AnimationPanel.h"
#include "../../SmolEngine/Source/ImGui/BuildPanel.h"
#include "../../SmolEngine/Source/ImGui/EditorConsole.h"
#include "../../SmolEngine/Source/ImGui/ImGuiLayer.h"
#include "../../SmolEngine/Source/ImGui/FileBrowser/imfilebrowser.h"

#include <../../SmolEngine/Libraries/cereal/include/cereal/cereal.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/types/polymorphic.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/archives/json.hpp>
#include <../../SmolEngine/Libraries/glm/glm/glm.hpp>


