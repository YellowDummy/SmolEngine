#pragma once
#include "Core/Core.h"

#include <meta/meta.hpp>
#include <meta/factory.hpp>

namespace SmolEngine
{
	struct CppScriptComponent;
	class Actor;

	class MetaContext
	{
		struct MetaData
		{
			meta::any  ClassInstance;

			meta::func OnBeginFunc;
			meta::func OnProcessFunc;
			meta::func OnDestroyFunc;

			meta::func OnCollBeginFunc;
			meta::func OnCollEndFunc;
			meta::func OnDebugDrawFunc;
		};

	public:
		static MetaContext* GetSingleton() { return s_Instance; }
		const std::unordered_map<std::string, MetaContext::MetaData>& GetMeta();

	private:
		template<typename T>
		bool AddClass(const std::string& name)
		{
			auto& it = m_MetaMap.find(name);
			if (it != m_MetaMap.end())
				return false;

			// Hashes
			size_t nameHash = m_Hasher(name);
			size_t onBeginHash = m_Hasher("OnBegin");
			size_t onProcessHash = m_Hasher("OnProcess");
			size_t onDestroyHash = m_Hasher("OnDestroy");
			size_t onCollBeginHash = m_Hasher("OnCollisionContact");
			size_t onCollExitHash = m_Hasher("OnCollisionExit");

			// registration
			auto factory = meta::reflect<T>(nameHash);
			factory.base<BehaviourPrimitive>();
			factory.func<&T::OnBegin>(onBeginHash);
			factory.func<&T::OnProcess>(onProcessHash);
			factory.func<&T::OnDestroy>(onDestroyHash);
			factory.func<&T::OnCollisionContact>(onCollBeginHash);
			factory.func<&T::OnCollisionExit>(onCollExitHash);

			// reflection
			MetaContext::MetaData metaData = {};
			metaData.ClassInstance = T();
			metaData.OnBeginFunc = meta::resolve(nameHash).func(onBeginHash);
			metaData.OnDestroyFunc = meta::resolve(nameHash).func(onDestroyHash);
			metaData.OnProcessFunc = meta::resolve(nameHash).func(onProcessHash);
			metaData.OnCollBeginFunc = meta::resolve(nameHash).func(onCollBeginHash);
			metaData.OnCollEndFunc = meta::resolve(nameHash).func(onCollExitHash);

			m_MetaMap[name] = std::move(metaData);
			return true;
		}

		void OnBegin(CppScriptComponent* comp);
		void OnUpdate(CppScriptComponent* comp, float deltaTime);
		void OnDestroy(CppScriptComponent* comp);
		void OnCollisionBegin(CppScriptComponent* comp, Actor* another, bool isTrigger);
		void OnCollisionEnd(CppScriptComponent* comp, Actor* another, bool isTrigger);
		void OnSceneReloaded(void* registry_);

	private:;

		inline static MetaContext*                     s_Instance = nullptr;
		std::hash<std::string_view>                    m_Hasher{};
		std::unordered_map<std::string, MetaData>      m_MetaMap;

		friend class ScriptingSystem;
	};
}