#pragma once
#include "Core/Core.h"
#include "ECS/Scene.h"

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL;
#endif
#include <Frostium3D/Common/RendererShared.h>

#include <unordered_map>
#include <string>
#include <entt/entity/registry.hpp>

namespace SmolEngine
{
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
		BeginSceneInfo                                        m_SceneInfo{};
		Scene                                                 m_Scenes[2];
		std::hash<std::string_view>                           m_Hash{};
	};
}