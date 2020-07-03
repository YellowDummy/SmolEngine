#pragma once

#ifdef PLATFORM_WIN
    #ifdef BUILD_DLL
         #define SMOL_ENGINE_API __declspec(dllexport)
    #else
         #define SMOL_ENGINE_API __declspec(dllimport)
    #endif
#else 
#error Win support only
#endif