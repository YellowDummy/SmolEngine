#include "stdafx.h"
#include "Core/AssetManager.h"
#include "Core/ECS/ComponentsCore.h"

#include "Core/ECS/Systems/AudioSystem.h"
#include "Core/ECS/Systems/UISystem.h"
#include "Core/Animation/AnimationClip.h"
#include "Core/Audio/AudioEngine.h"

namespace SmolEngine
{
	Ref<AssetManager> AssetManager::s_Instance = std::make_shared<AssetManager>();
	Ref<ShaderLib> AssetManager::s_ShaderLib = std::make_shared<ShaderLib>();

	AssetManager::AssetManager()
	{
		
	}

	void AssetManager::Reload2DTextures(entt::registry& registry, const std::unordered_map<std::string, std::string>& assetMap)
	{
		const auto& view = registry.view<Texture2DComponent>();
		view.each([&](Texture2DComponent& texture)
		{
			auto search = assetMap.find(texture.FileName);
			if (search != assetMap.end())
			{
				texture.Texture = Texture2D::Create(search->second);
			}
		});
	}

	void AssetManager::Reload2DAnimations(entt::registry& registry)
	{
		const auto& view = registry.view<Animation2DComponent>();
		view.each([&](Animation2DComponent& anim)
			{
				for (const auto& pair : anim.m_Clips)
				{
					auto& [key, clip] = pair;

					for (const auto& framePair : clip->m_Frames)
					{
						const auto& [key, frame] = framePair;

						if (!PathCheck(frame->TexturePath, frame->FileName))
						{
							NATIVE_ERROR("Animation2D reload: texture not found, path: {}!", frame->FileName.c_str());
							continue;
						}

						frame->Texture = Texture2D::Create(frame->TexturePath);
					}
				}

			});
	}

	void AssetManager::ReloadAudioClips(entt::registry& registry, AudioEngine* engine)
	{
		const auto& view = registry.view<AudioSourceComponent>();
		view.each([&](AudioSourceComponent& audio)
		{
			// Relaoding Audio Clips
			AudioSystem::ReloadAllClips(audio, engine);
		});
	}

	void AssetManager::ReloadCanvases(entt::registry& registry)
	{
		const auto& view = registry.view<CanvasComponent>();
		view.each([&](CanvasComponent& canvas)
		{
			UISystem::ReloadElements(canvas);
		});
	}

	bool AssetManager::PathCheck(std::string& path, const std::string& fileName)
	{
		if (path.empty() || fileName.empty())
		{
			CONSOLE_ERROR(std::string("Invalid Path!"));
			return false;
		}

		if (!IsPathValid(path))
		{
			if (!ChangeFilePath(fileName, path))
			{
				CONSOLE_ERROR(std::string("Asset ") + fileName + std::string(" not found"));
				return false;
			}
		}

		return true;
	}

	bool AssetManager::ChangeFilePath(const std::string& fileName, std::string& pathToChange)
	{
		using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

#ifdef SMOLENGINE_EDITOR // Dodgy implementation

		for (const auto& dirEntry : recursive_directory_iterator(std::string("../GameX/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().u8string();
				return true;
			}
		}

		for (const auto& dirEntry : recursive_directory_iterator(std::string("../SmolEngine/Assets/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().u8string();
				return true;
			}
		}

		for (const auto& dirEntry : recursive_directory_iterator(std::string("../SmolEngine-Editor/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().u8string();
				return true;
			}
		}
#else
		for (const auto& dirEntry : recursive_directory_iterator(std::string("C:/Dev/SmolEngine/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().u8string();
				return true;
			}
		}
#endif


		return false;
	}

	bool AssetManager::IsPathValid(const std::string& path)
	{
		return std::filesystem::exists(path);
	}

	std::string AssetManager::GetCachedPath(const std::string& filePath, CachedPathType type)
	{
		std::filesystem::path p = filePath;
		std::filesystem::path path;
		switch (type)
		{
		case SmolEngine::CachedPathType::Shader:
			path = p.parent_path() / "Cached" / (p.filename().string() + ".shader_cached");
			break;
		case SmolEngine::CachedPathType::Pipeline:
			path = p.parent_path() / "Cached" / (p.filename().string() + ".pipeline_cached");
			break;
		default:
			path = p.parent_path() / "Cached" / (p.filename().string() + ".shader_cached");
			break;
		}

		return path.string();
	}
}