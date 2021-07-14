#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#include <mono/jit/jit.h>

namespace SmolEngine
{
    struct Vector3
    {
        float X;
        float Y;
        float Z;
    };

    struct TransformComponentCSharp
    {
        Vector3 WorldPos;
        Vector3 Rotation;
        Vector3 Scale;
    };

    struct HeadComponentCSharp
    {
        MonoString* Name;
        MonoString* Tag;
    };
}