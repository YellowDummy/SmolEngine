#include "stdafx.h"
#include "SceneData.h"

namespace SmolEngine
{
static const size_t reserveSize = 259;

	SceneData::SceneData(const std::string& filePath, const std::string& fileName)
	{
		m_filePath = filePath;
		m_fileName = fileName;
		m_Name = fileName;
		m_ID = std::hash<std::string>{}(filePath);

		m_Registry = entt::registry();
		m_Entity = m_Registry.create();
	}

	void SceneData::operator=(const SceneData& other)
	{
		m_ActorPool = other.m_ActorPool;
		m_AssetMap = other.m_AssetMap;
		m_Entity = other.m_Entity;
		m_Gravity.x = other.m_Gravity.x;
		m_Gravity.y = other.m_Gravity.y;
		m_ID = other.m_ID;
		m_filePath = other.m_filePath;
		m_fileName = other.m_fileName;
		m_Name = other.m_Name;
		m_AmbientStrength = other.m_AmbientStrength;
	}

	void SceneData::Prepare()
	{
		m_ActorPool.reserve(reserveSize);
		m_ActorList.reserve(reserveSize);
	}
}