#pragma once
#include "Core/Core.h"
#include "ECS/Scene.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

#include <unordered_map>
#include <string>

namespace SmolEngine
{
	struct WorldAdminStateSComponent
	{
		WorldAdminStateSComponent();

		~WorldAdminStateSComponent();

		WorldAdminStateSComponent(const WorldAdminStateSComponent& another);

		static WorldAdminStateSComponent* GetSingleton() { return s_Instance; }

		// Data

		inline static WorldAdminStateSComponent* s_Instance = nullptr;
		bool                                     m_InPlayMode = false;

		uint32_t                                 m_NumLoadedScene = 0;
		uint32_t                                 m_MaxAssetPoolLiftime = 3; // in NumLoadedScene
		size_t                                   m_ActiveSceneID = 0;
		std::hash<std::string_view>              m_Hash{};
		std::unordered_map<size_t, Ref<Mesh>>    m_MeshMap;
		std::unordered_map<size_t, Ref<Texture>> m_TexturesMap;
		std::unordered_map<size_t, Scene>        m_SceneMap;
	};
}