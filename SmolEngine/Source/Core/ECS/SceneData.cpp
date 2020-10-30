#include "stdafx.h"
#include "SceneData.h"

namespace SmolEngine
{
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
}