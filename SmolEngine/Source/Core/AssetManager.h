#pragma once

#include "Core/Core.h"
#include "Renderer/Shader.h"

#include <unordered_map>
#include <entt.hpp>

namespace SmolEngine
{
	class AudioEngine;

	enum class CachedPathType
	{
		Shader,
		Pipeline
	};

	class AssetManager
	{
	public:

		AssetManager();

		~AssetManager() = default;

		AssetManager(const AssetManager& other) = delete;

		// Reload
		static void Reload2DTextures(entt::registry& registry, const std::unordered_map<std::string, std::string>& assetMap);

		static void Reload2DAnimations(entt::registry& registry);

		static void ReloadAudioClips(entt::registry& registry, AudioEngine* engine);

		static void ReloadCanvases(entt::registry& registry);

		//Helpers

		static bool PathCheck(std::string& path, const std::string& fileName);

		static bool ChangeFilePath(const std::string& fileName, std::string& pathToChange);

		static bool IsPathValid(const std::string& path);


		static std::string GetCachedPath(const std::string& filePath, CachedPathType type);

		static Ref<ShaderLib> GetShaderLib() { return s_ShaderLib; }

	private:

		static Ref<AssetManager>       s_Instance;
		static Ref<ShaderLib>          s_ShaderLib;

	};
}
