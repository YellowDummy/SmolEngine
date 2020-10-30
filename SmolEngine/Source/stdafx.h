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


#include "../../SmolEngine/Source/Core/ECS/WorldAdmin.h"
#include "../../SmolEngine/Source/Core/ECS/Actor.h"

#include "../../SmolEngine/Source/Core/Audio/AudioEngine.h"

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

#include "Core/ECS/Components/BehaviourComponent.h"

#include "Core/ECS/ComponentTuples/DefaultBaseTuple.h"
#include "Core/ECS/ComponentTuples/PhysicsBaseTuple.h"
#include "Core/ECS/ComponentTuples/CameraBaseTuple.h"
#include "Core/ECS/ComponentTuples/ResourceTuple.h"
#include "Core/ECS/ComponentTuples/SingletonTuple.h"

#include "Core/ECS/Systems/RendererSystem.h"
#include "Core/ECS/Systems/Box2DPhysicsSystem.h"
#include "Core/ECS/Systems/AudioSystem.h"
#include "Core/ECS/Systems/Animation2DSystem.h"
#include "Core/ECS/Systems/CameraSystem.h"
#include "Core/ECS/Systems/CommandSystem.h"
#include "Core/ECS/Systems/UISystem.h"

#include "../../SmolEngine/Source/Core/Application.h"

#include "Core/Scripting/SystemRegistry.h"

#include "../../SmolEngine/Source/Core/ImGui/AnimationPanel.h"
#include "../../SmolEngine/Source/Core/ImGui/BuildPanel.h"
#include "../../SmolEngine/Source/Core/ImGui/EditorConsole.h"
#include "../../SmolEngine/Source/Core/ImGui/ImGuiLayer.h"
#include "../../SmolEngine/Source/Core/ImGui/FileBrowser/imfilebrowser.h"

#include <../../SmolEngine/Libraries/cereal/include/cereal/cereal.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/types/polymorphic.hpp>
#include <../../SmolEngine/Libraries/cereal/include/cereal/archives/json.hpp>
#include <../../SmolEngine/Libraries/glm/glm/glm.hpp>


