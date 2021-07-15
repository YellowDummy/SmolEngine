#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#include <mono/jit/jit.h>

namespace SmolEngine
{
    struct TransformComponentCSharp
    {
        glm::vec3 WorldPos;
        glm::vec3 Rotation;
        glm::vec3 Scale;
    };

    struct HeadComponentCSharp
    {
        MonoString* Name = nullptr;
        MonoString* Tag = nullptr;
    };
}