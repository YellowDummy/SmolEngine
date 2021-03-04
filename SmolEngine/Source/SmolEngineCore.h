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

#include "ECS/ComponentTuples/SingletonTuple.h"
#include "ECS/ComponentsCore.h"

#include "ECS/Systems/Physics2DSystem.h"
#include "ECS/Systems/RendererSystem.h"

#include "Scripting/BehaviourPrimitive.h"

#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include "Renderer/SubTexture.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"
