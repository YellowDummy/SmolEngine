#include "stdafx.h"
#include "ECS/SceneRegisrty.h"

namespace SmolEngine
{
	void SceneRegisrty::Init()
	{
		m_Registry = entt::registry();
		m_Entity = m_Registry.create();
	}

	void SceneRegisrty::Free()
	{
		m_Registry.clear();
	}
}