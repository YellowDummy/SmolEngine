#pragma once

#include "Core/Core.h"

#include <unordered_map>
#include <glm/glm.hpp>
#include <entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	class Actor;

	struct SceneData
	{
		SceneData() = default;

		SceneData(const std::string& filePath, const std::string& name);

		void operator=(const SceneData& other);

	public:

		std::unordered_map<size_t, Ref<Actor>> m_ActorPool;
		std::unordered_map<std::string, std::string> m_AssetMap;

		glm::vec2 m_Gravity = glm::vec2(0.0f, -9.81f);
		entt::registry m_Registry;
		entt::entity m_Entity;

		float m_AmbientStrength = 1.0f;

	private:

		std::string m_filePath = "";
		std::string m_fileName = "";
		std::string m_Name = std::string("");
		size_t m_ID = 0;

	private:

		friend class EditorLayer;
		friend class WorldAdmin;
		friend class Scene;
		friend class SettingsWindow;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_ActorPool, m_AssetMap, m_Entity, m_Gravity.x, m_Gravity.y, m_ID, m_filePath, m_fileName, m_Name, m_AmbientStrength);
		}
	};
}