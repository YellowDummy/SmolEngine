#pragma once

#include "Core/Core.h"

#ifdef WIN32
#include <malloc.h>
#include <crtdbg.h>
#endif 

#include <Frostium3D/Common/SLog.h>
#include <foundation/PxAllocatorCallback.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxErrors.h>

namespace SmolEngine
{
    class PhysXAllocator : public physx::PxAllocatorCallback
    {
    public:

        void* allocate(size_t size, const char* typeName, const char* filename,  int line) override
        {
            return _aligned_malloc(size, 16);
        }

        void deallocate(void* ptr) override
        {
            if (!ptr)
                return;

            _aligned_free(ptr);
        }
    };

    class PhysXErrorCallback : public physx::PxErrorCallback
    {
    public:

        void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
        {
            switch (code)
            {
            case physx::PxErrorCode::eNO_ERROR:
                break;
            case physx::PxErrorCode::eDEBUG_INFO:
                NATIVE_INFO("message: {}, file: {}, line: {}", message, file, line);
                break;
            case physx::PxErrorCode::eDEBUG_WARNING:
                NATIVE_WARN("message: {}, file: {}, line: {}", message, file, line);
                break;
            case physx::PxErrorCode::eINVALID_PARAMETER:
                NATIVE_ERROR("message: {}, file: {}, line: {}", message, file, line);
                break;
            case physx::PxErrorCode::eINVALID_OPERATION:
                NATIVE_ERROR("message: {}, file: {}, line: {}", message, file, line);
                break;
            case physx::PxErrorCode::eOUT_OF_MEMORY:
                NATIVE_ERROR("message: {}, file: {}, line: {}", message, file, line);
                break;
            case physx::PxErrorCode::eINTERNAL_ERROR:
                NATIVE_ERROR("message: {}, file: {}, line: {}", message, file, line);
                break;
            case physx::PxErrorCode::eABORT:
                NATIVE_ERROR("message: {}, file: {}, line: {}", message, file, line);
                break;
            case physx::PxErrorCode::ePERF_WARNING:
                NATIVE_ERROR("message: {}, file: {}, line: {}", message, file, line);
                break;
            case physx::PxErrorCode::eMASK_ALL:
                break;
            default:
                break;
            }
        }
    };
}