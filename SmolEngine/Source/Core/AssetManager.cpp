#include "stdafx.h"
#include "Core/AssetManager.h"

#include "ECS/ComponentsCore.h"
#include "ECS/Systems/AudioSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Systems/JobsSystem.h"
#include "ECS/Systems/CommandSystem.h"
#include "ECS/SceneData.h"

#include "Renderer/MaterialLibrary.h"

#include "Animation/AnimationClip2D.h"
#include "Audio/AudioEngine.h"
#include "ImGui/EditorConsole.h"

namespace SmolEngine
{
	Ref<AssetManager> AssetManager::s_Instance = std::make_shared<AssetManager>();

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
				//texture.Texture = Texture::Create(search->second);
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

						//frame->Texture = Texture::Create(frame->TexturePath);
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

	void AssetManager::ReloadMeshMaterials(entt::registry& registry, SceneData* data)
	{
		MaterialLibrary* instance = MaterialLibrary::GetSinglenton();
		instance->Reset();

		// Default Material = ID 0
		MaterialCreateInfo materialCI = {};
		instance->Add(&materialCI, "Default Material");

		std::vector<std::string> usedMaterials;
		{
			const auto& view = registry.view<MeshComponent>();
			view.each([&](MeshComponent& component)
				{
					bool found = false;
					for (auto& meshData : component.MeshData)
					{
						if (usedMaterials.size() > 1)
						{
							bool found = false;
							for (auto& usedMaterials : usedMaterials)
							{
								if (meshData.MaterialPath == usedMaterials)
								{
									found = true;
									break;
								}
							}

							if (!found)
								usedMaterials.push_back(meshData.MaterialPath);

							continue;
						}

						usedMaterials.push_back(meshData.MaterialPath);
					}

				});

		}

		data->m_MaterialPaths = std::move(usedMaterials);

		// Scene materials
		for (auto& path : data->m_MaterialPaths)
		{
			materialCI = {};
			bool load = instance->Load(path, materialCI);
			if (load)
			{
				instance->Add(&materialCI, path);
				continue;
			}

			NATIVE_ERROR("Material {] not found!", path);
		}

		// Load Meshes and updates components
		const auto& view = registry.view<MeshComponent>();
		view.each([&](MeshComponent& component)
		{
			if (!component.FilePath.empty())
			{
				CommandSystem::LoadMeshComponent(&component, component.FilePath, false);

				if ((component.MeshData.size() > 0))
				{
					uint32_t index = 0;
					int32_t id = instance->GetMaterialID(component.MeshData[index].MaterialHash);
					component.MeshData[index].MaterialID = id;
					index++;

					for (auto& sub : component.Mesh->GetSubMeshes())
					{
						int32_t id = instance->GetMaterialID(component.MeshData[index].MaterialHash);
						component.MeshData[index].MaterialID = id;
						index++;
					}
				}
			}

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

#ifdef SMOLENGINE_EDITOR

		for (const auto& dirEntry : recursive_directory_iterator(std::string("../GameX/")))
		{
			if (dirEntry.path().filename().stem() == fileName)
			{
				pathToChange = dirEntry.path().string();
				return true;
			}
		}
#else
		for (const auto& dirEntry : recursive_directory_iterator(std::string("C:/Dev/SmolEngine/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().string();
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