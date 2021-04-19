#pragma once
#include <memory>

#include <Frostium3D/Common/SLog.h>

namespace SmolEngine
{
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
}
