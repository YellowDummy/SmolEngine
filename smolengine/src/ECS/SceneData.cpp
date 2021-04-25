#include "stdafx.h"
#include "ECS/SceneData.h"

namespace SmolEngine
{
	void SceneData::Init()
	{
		m_Registry = entt::registry();
		m_Entity = m_Registry.create();
	}

	void SceneData::Free()
	{
		m_Registry.clear();
	}
}