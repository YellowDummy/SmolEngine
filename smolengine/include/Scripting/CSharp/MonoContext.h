#pragma once
#include "Core/Core.h"
#include <vector>
#include <unordered_map>

typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoClass MonoClass;
typedef struct _MonoImage MonoImage;

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
		void                        Track();

	private:
		void                        Shutdown();
		void                        LoadAssembly(bool is_initialization = false);
		void                        ResolveFunctions();
		void                        ResolveClasses();
		void                        GetClassNames();
		void                        OnRecompilation();
		void                        LoadMonoImage();
		void                        LoadDomain();

	public:
		MonoDomain*                                 m_Domain = nullptr;
		MonoDomain*                                 m_RootDomain = nullptr;
		MonoAssembly*                               m_CSharpAssembly = nullptr;
		MonoImage*                                  m_Image = nullptr;
		std::string                                 m_DLLPath = "../vendor/mono/CSharp/Debug/CSharp.exe";
		std::filesystem::file_time_type             m_LastWriteTime;
		std::vector<std::string>                    m_ClassNames;
		std::unordered_map<ClassDefs, MonoClass*>   m_DefaultClasses;
												   
	private:
		inline static MonoContext*                  s_Instance = nullptr;
	};
}