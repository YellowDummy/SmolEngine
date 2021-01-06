#pragma once

#include "Core/Application.h"
#include "Core/SLog.h"
#include "Core/Tools.h"
#include "Core/Time.h"
#include "Core/Input.h"
#include "Core/Layer.h"
#include "Core/InputCodes.h"
#include "Core/EngineCommand.h"

#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/InputEvent.h"
#include "Core/Events/MouseEvent.h"

#include "Core/ECS/Actor.h"
#include "Core/ECS/WorldAdmin.h"

#include "Core/ImGui/ImGuiLayer.h"
#include "Core/ImGui/EditorConsole.h"

#include "Core/ECS/ComponentTuples/DefaultBaseTuple.h"
#include "Core/ECS/ComponentTuples/CameraBaseTuple.h"
#include "Core/ECS/ComponentTuples/SingletonTuple.h"
#include "Core/ECS/ComponentsCore.h"

#include "Core/ECS/Systems/Physics2DSystem.h"
#include "Core/ECS/Systems/RendererSystem.h"

#include "Core/Scripting/BehaviourPrimitive.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/SubTexture.h"
#include "Core/Renderer/Buffer.h"
#include "Core/Renderer/Camera.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/OpenGL/OpenglBuffer.h"
#include "Core/Renderer/OpenGL/OpenglShader.h"
#include "Core/Renderer/OpenGL/OpenglVertexArray.h"
#include "Core/Renderer/OpenGL/OpenglRendererAPI.h"
#include "Core/Renderer/OpenGL/OpenglContext.h"
#include "Core/Renderer/OpenGL/OpenglTexture.h"
