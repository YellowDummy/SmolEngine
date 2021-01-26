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


#include "ECS/WorldAdmin.h"
#include "ECS/Actor.h"

#include "Audio/AudioEngine.h"

#include "Renderer/Renderer2D.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/SubTexture.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"

#include "Renderer/OpenGL/OpenglRendererAPI.h"
#include "Renderer/OpenGL/OpenglShader.h"
#include "Renderer/OpenGL/OpenglTexture.h"

#ifndef SMOLENGINE_OPENGL_IMPL
#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanContext.h"
#endif

#include "ECS/Components/BehaviourComponent.h"
#include "ECS/Components/Light2DSourceComponent.h"

#include "ECS/ComponentTuples/DefaultBaseTuple.h"
#include "ECS/ComponentTuples/CameraBaseTuple.h"
#include "ECS/ComponentTuples/SingletonTuple.h"

#include "../../SmolEngine/Source/Core/Application.h"

#include "ImGui/AnimationPanel.h"
#include "ImGui/BuildPanel.h"
#include "ImGui/EditorConsole.h"
#include "ImGui/ImGuiLayer.h"
#include "ImGui/FileBrowser/imfilebrowser.h"

#include <../../SmolEngine/Libraries/cereal/include/cereal/cereal.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/types/polymorphic.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/archives/json.hpp>
#include <../../SmolEngine/Libraries/glm/glm/glm.hpp>


