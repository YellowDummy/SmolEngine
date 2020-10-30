#include "stdafx.h"
#include "BaseTupleBehaviour.h"

namespace SmolEngine
{
	DefaultBaseTuple& BaseTupleBehaviour::GetDefaultBaseTuple()
	{
		return *WorldAdmin::GetScene()->GetTuple<DefaultBaseTuple>(*m_Actor);
	}

	ResourceTuple* BaseTupleBehaviour::GetResourceTuple()
	{
		return WorldAdmin::GetScene()->GetTuple<ResourceTuple>(*m_Actor);
	}

	void BaseTupleBehaviour::SetTexture(Ref<Texture> texture)
	{

	}
}