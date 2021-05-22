#pragma once
#include <memory>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
    extern "C++"
    {
        template<typename T>
        using Scope = std::unique_ptr<T>;

        template<typename T>
        using Ref = std::shared_ptr<T>;
    }
}
