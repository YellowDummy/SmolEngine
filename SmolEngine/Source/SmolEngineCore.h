#pragma once

#include "Core/Application.h"
#include "Core/SLog.h"
#include "Core/Tools.h"
#include "Core/Time.h"
#include "Core/Input.h"
#include "Core/Layer.h"
#include "Core/InputCodes.h"
#include "Core/EngineCommand.h"

#include "Events/ApplicationEvent.h"
#include "Events/InputEvent.h"
#include "Events/MouseEvent.h"

#include "ECS/Actor.h"
#include "ECS/WorldAdmin.h"

#include "ImGui/ImGuiLayer.h"
#include "ImGui/EditorConsole.h"

#include "ECS/ComponentTuples/DefaultBaseTuple.h"
#include "ECS/ComponentTuples/CameraBaseTuple.h"
#include "ECS/ComponentTuples/SingletonTuple.h"
#include "ECS/ComponentsCore.h"

#include "ECS/Systems/Physics2DSystem.h"
#include "ECS/Systems/RendererSystem.h"

#include "Scripting/BehaviourPrimitive.h"

#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include "Renderer/SubTexture.h"
#include "Renderer/Buffer.h"
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/OpenGL/OpenglBuffer.h"
#include "Renderer/OpenGL/OpenglShader.h"
#include "Renderer/OpenGL/OpenglVertexArray.h"
#include "Renderer/OpenGL/OpenglRendererAPI.h"
#include "Renderer/OpenGL/OpenglContext.h"
#include "Renderer/OpenGL/OpenglTexture.h"
