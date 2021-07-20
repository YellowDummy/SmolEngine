#pragma once
#include "Core/Core.h"
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoClass MonoClass;
typedef struct _MonoImage MonoImage;
typedef struct _MonoMethod MonoMethod;

#ifndef _MONO_UTILS_FORWARD_
#define _MONO_UTILS_FORWARD_

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoJitInfo MonoJitInfo;

#endif

namespace SmolEngine
{
	class Actor;
	struct CSharpScriptComponent;

	class MonoContext
	{
	public:
		MonoContext();

		void                        Create();
		void                        Shutdown();
		void                        Track();
		bool                        IsRunning();
		static MonoContext*         GetSingleton();
		MonoDomain*                 GetDomain();
		void                        SetOnReloadCallback(const std::function<void()>& callback);
							
	private:
		enum class InternalClassType
		{
			BehaviourPrimitive,
			Actor,
			UnitTests
		};

		struct MetaData
		{
			MonoClass*              pClass = nullptr;
			MonoMethod*             pOnUpdate = nullptr;
			MonoMethod*             pOnBegin = nullptr;
			MonoMethod*             pOnDestroy = nullptr;
			MonoMethod*             pOnCollisionBegin = nullptr;
			MonoMethod*             pOnCollisionEnd = nullptr;
		};				
									                 
		void                         LoadAssembly(bool is_initialization = false);
		void                         ResolveFunctions();
		void                         ResolveClasses();
		void                         ResolveMeta();
		void                         OnRecompilation();
		void                         RunTest();
		void                         LoadMonoImage();
		void                         LoadDomain();
		void*                        CreateClassInstance(const std::string& class_name, const Ref<Actor>& actor);
		void*                        GetMethod(const char* signature, const char* class_name, MonoClass* p_class);
							         
		void                         OnBegin(const CSharpScriptComponent* comp);
		void                         OnUpdate(const CSharpScriptComponent* comp);
		void                         OnInternalUpdate(float delta);
		void                         OnDestroy(const CSharpScriptComponent* comp);
		void                         OnCollisionBegin(const CSharpScriptComponent* comp, Actor* another, bool isTrigger);
		void                         OnCollisionEnd(const CSharpScriptComponent* comp, Actor* another, bool isTrigger);
		void                         OnReload(CSharpScriptComponent* comp);  /* scene reload */
		const MonoContext::MetaData* GetMeta(const CSharpScriptComponent* comp) const;
												  
	private:		       
		inline static MonoContext*                        s_Instance = nullptr;
		MonoDomain*                                       m_Domain = nullptr;
		MonoDomain*                                       m_RootDomain = nullptr;
		MonoAssembly*                                     m_CSharpAssembly = nullptr;
		MonoImage*                                        m_Image = nullptr;
		std::function<void()>                             m_Callback = nullptr;
		std::string                                       m_DLLPath = "../vendor/mono/CSharp/Debug/CSharp.dll";
		std::filesystem::file_time_type                   m_LastWriteTime;
		std::unordered_map<std::string, MetaData>         m_MetaMap;
		std::unordered_map<InternalClassType, MonoClass*> m_InternalClasses;

		friend class ScriptingSystem;
		friend class EditorLayer;
	};   
}