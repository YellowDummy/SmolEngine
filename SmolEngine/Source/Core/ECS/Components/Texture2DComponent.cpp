#include "stdafx.h"
#include "Texture2DComponent.h"

#include "Core/Renderer/Texture.h"

namespace SmolEngine
{
	Texture2DComponent::Texture2DComponent()
	{

	}

	Texture2DComponent::Texture2DComponent(const std::string& filePath, const glm::vec4& color)
		:
		TexturePath(filePath),
		Color(color)
	{
		Texture = Texture2D::Create(filePath);
	}
}