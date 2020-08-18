#pragma once

#include "Core/Core.h"
#include "Core/Renderer/Shader.h"

namespace SmolEngine
{
	class AssetManager
	{
	public:

		AssetManager();
		~AssetManager() = default;
		AssetManager(const AssetManager& other) = delete;

		static Ref<ShaderLib> GetShaderLib() { return s_ShaderLib; }
	private:
		static Ref<AssetManager> s_Instance;
		static Ref<ShaderLib> s_ShaderLib;

	};
}
