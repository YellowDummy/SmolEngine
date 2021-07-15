#pragma once
#include "Core/Core.h"

#ifndef _MONO_UTILS_FORWARD_
#define _MONO_UTILS_FORWARD_

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoJitInfo MonoJitInfo;

#endif

namespace SmolEngine
{
	class MonoContext
	{
	public:
		MonoContext();

		static MonoDomain* GetDomain();

	private:
		inline static MonoContext* s_Instance = nullptr;
		MonoDomain*                m_Domain = nullptr;
	};
}