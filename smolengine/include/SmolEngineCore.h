#pragma once

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif

#include "Core/Engine.h"
#include "Core/Layer.h"
#include "Core/EntryPoint.h"

#include "ECS/Actor.h"
#include "ECS/WorldAdmin.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Systems/Physics2DSystem.h"
#include "ECS/Systems/RendererSystem.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Systems/PhysicsSystem.h"

#include <Frostium3D/GraphicsContext.h>
#include <Frostium3D/DebugRenderer.h>
#include <Frostium3D/Common/SLog.h>
#include <Frostium3D/Common/Input.h>

#include "Scripting/BehaviourPrimitive.h"
