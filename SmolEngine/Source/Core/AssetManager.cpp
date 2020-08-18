#include "stdafx.h"
#include "Core/AssetManager.h"

namespace SmolEngine
{
	Ref<AssetManager> AssetManager::s_Instance = std::make_shared<AssetManager>();
	Ref<ShaderLib> AssetManager::s_ShaderLib = std::make_shared<ShaderLib>();

	AssetManager::AssetManager()
	{
		
	}
}