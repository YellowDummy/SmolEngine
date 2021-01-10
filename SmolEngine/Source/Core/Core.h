#pragma once
#include <memory>

#ifdef PLATFORM_WIN
#if SE_STATIC_LINK
    #ifdef BUILD_DLL
         #define SMOL_ENGINE_API __declspec(dllexport)
    #else
         #define SMOL_ENGINE_API __declspec(dllimport)
    #endif
#else 
     #define SMOL_ENGINE_API
#endif
#else 
    #error Win support only
#endif


namespace SmolEngine
{
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
}
