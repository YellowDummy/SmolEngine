#pragma once
#include "Core/Core.h"
#include "ECS/Scene.h"

#include <Frostium3D/Common/Mesh.h>
#include <Frostium3D/Common/Texture.h>

#include <unordered_map>
#include <string>
#include <entt/entity/registry.hpp>

namespace SmolEngine
{
	static const uint32_t maxScenes = 10;

	struct WorldAdminStateSComponent
	{
		WorldAdminStateSComponent();
		~WorldAdminStateSComponent();
		// Dummy c-tors - required by EnTT
		WorldAdminStateSComponent(WorldAdminStateSComponent& another) {}
		WorldAdminStateSComponent(WorldAdminStateSComponent&& other) {}
		WorldAdminStateSComponent& operator=(WorldAdminStateSComponent other) { return *this; }

		static WorldAdminStateSComponent* GetSingleton() { return s_Instance; }
														    
		inline static WorldAdminStateSComponent*              s_Instance = nullptr;
		bool                                                  m_InPlayMode = false;
		entt::registry*                                       m_CurrentRegistry = nullptr;
		uint32_t                                              m_ActiveSceneID = 0;
		Scene                                                 m_Scenes[maxScenes];
		std::hash<std::string_view>                           m_Hash{};
		std::unordered_map<uint32_t, Ref<Frostium::Mesh>>     m_MeshMap;
		std::unordered_map<uint32_t, Ref<Frostium::Texture>>  m_TexturesMap;
	};
}