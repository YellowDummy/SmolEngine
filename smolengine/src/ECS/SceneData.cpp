#include "stdafx.h"
#include "ECS/SceneData.h"

namespace SmolEngine
{
static const size_t reserveSize = 259;

	SceneData::SceneData(const std::string& filePath, const std::string& fileName)
	{
		m_filePath = filePath;
		m_Name = fileName;
		m_ID = std::hash<std::string>{}(filePath);

		m_Registry = entt::registry();
		m_Entity = m_Registry.create();
	}

	void SceneData::operator=(const SceneData& other)
	{
		m_ActorPool = other.m_ActorPool;
		m_Entity = other.m_Entity;
		m_Gravity.x = other.m_Gravity.x;
		m_Gravity.y = other.m_Gravity.y;
		m_ID = other.m_ID;
		m_filePath = other.m_filePath;
		m_Name = other.m_Name;
		m_AmbientStrength = other.m_AmbientStrength;

		m_AssetMap = std::move(other.m_AssetMap);
		m_MaterialPaths = std::move(other.m_MaterialPaths);
	}

	void SceneData::Prepare()
	{
		m_ActorPool.reserve(reserveSize);
		m_ActorList.reserve(reserveSize);
	}
}