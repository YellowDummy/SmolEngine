#pragma once
#pragma once

#include <string>
#include <unordered_map>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct SceneConfigData
	{
		std::string FilePath = "";
		std::string FileName = "";

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(FilePath, FileName);
		}
	};

	struct ProjectConfigSComponent
	{
		ProjectConfigSComponent();

		~ProjectConfigSComponent();

		/// Data

		inline static ProjectConfigSComponent*           Instance = nullptr;
		std::unordered_map<uint32_t, SceneConfigData>    m_Scenes;
		std::string                                      m_AssetFolder = "";

		/// Getters

		static ProjectConfigSComponent* Get();

	private:

		friend class cereal::access;
		friend class BuildPanel;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Scenes, m_AssetFolder);
		}
	};
}