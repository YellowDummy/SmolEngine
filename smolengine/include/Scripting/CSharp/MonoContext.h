#pragma once
#include "Core/Core.h"
#include <vector>
#include <unordered_map>

typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoClass MonoClass;

#ifndef _MONO_UTILS_FORWARD_
#define _MONO_UTILS_FORWARD_

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoJitInfo MonoJitInfo;

#endif

namespace SmolEngine
{
	enum class ClassDefs
	{
		Actor,
		Reflection
	};

	class MonoContext
	{
	public:
		MonoContext();

		bool                        IsRunning();
		static MonoContext*         GetSingleton();
		void                        OnRecompilation();

	private:
		void                        Run();
		void                        Shutdown();
		void                        CreateAssembly();
		void                        ResolveFunctions();
		void                        ResolveClasses();
		void                        GetClassNames();

	public:
		MonoDomain*                                 m_Domain = nullptr;
		MonoAssembly*                               m_CSharpAssembly = nullptr;
		std::vector<std::string>                    m_ClassNames;
		std::unordered_map<ClassDefs, MonoClass*>   m_DefaultClasses;
												   
	private:
		inline static MonoContext*                  s_Instance = nullptr;
	};
}