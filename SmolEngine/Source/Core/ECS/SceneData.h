#pragma once

#include "Core/Core.h"

#include <unordered_map>
#include <glm/glm.hpp>
#include <entt.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Actor;

	struct SceneData
	{
		SceneData() = default;

		void operator=(const SceneData& other);

	public:

		std::unordered_map<size_t, Ref<Actor>> m_ActorPool;
		glm::vec2 m_Gravity = glm::vec2(0.0f, -9.81f);

		entt::registry m_Registry;
		float m_AmbientStrength = 1.0f;

	private:

		std::string m_filePath;
		std::string m_fileName;

		std::string m_Name = std::string("");
		size_t m_ID = 0;

	private:

		friend class EditorLayer;
		friend class Scene;
		friend class SettingsWindow;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_ActorPool, m_Gravity.x, m_Gravity.y, m_ID, m_filePath, m_fileName, m_Name, m_AmbientStrength);
			archive.serializeDeferments();
		}
	};
}