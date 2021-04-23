#include "stdafx.h"
#include "ECS/WorldAdmin.h"

#include "ECS/ComponentsCore.h"
#include "ECS/Systems/RendererSystem.h"
#include "ECS/Systems/Physics2DSystem.h"
#include "ECS/Systems/AudioSystem.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Components/Singletons/AudioEngineSComponent.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "ECS/Components/Singletons/ProjectConfigSComponent.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"
#include "ECS/Components/Singletons/ScriptingSystemStateSComponent.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"

#include <Frostium3D/Renderer.h>
#include <Frostium3D/Renderer2D.h>
#include <Frostium3D/MaterialLibrary.h>
#include <Frostium3D/Utils/Utils.h>
#include <Frostium3D/Common/Mesh.h>
#include <Frostium3D/Common/Texture.h>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <box2d/b2_world.h>

namespace SmolEngine
{
	WorldAdmin::WorldAdmin()
	{
		s_World = this;
	}

	void WorldAdmin::Init()
	{
		LoadStaticComponents();
		m_State->m_InPlayMode = false;
	}

	void WorldAdmin::ShutDown()
	{

	}

	void WorldAdmin::OnBeginWorld()
	{
		SceneData& sceneData = GetActiveScene()->GetSceneData();
#ifdef SMOLENGINE_EDITOR
		if (!Frostium::Utils::IsPathValid(sceneData.m_filePath))
		{
			NATIVE_ERROR("Failed to start the game!");
			return;
		}
		// We save the current scene before starting the simulation
		Save(sceneData.m_filePath);
#endif
		Box2DWorldSComponent* world = Box2DWorldSComponent::Get();
		// Setting Box2D Callbacks
		Physics2DSystem::OnBegin(world);
		// Creating rigidbodies and joints
		{
			const auto& view = sceneData.m_Registry.view<TransformComponent, Body2DComponent>();
			view.each([&](TransformComponent& tranform, Body2DComponent& body)
			{
					Physics2DSystem::CreateBody(&body, &tranform, &world->World, GetActiveScene()->FindActorByID(body.ActorID));
			});
		}

		// Finding which animation / audio clip should play on awake
		AudioSystem::OnAwake(&AudioEngineSComponent::Get()->Engine);
		// Sending start callback to all enabled scripts
		ScriptingSystem::OnBegin();
		m_State->m_InPlayMode = true;
	}

	void WorldAdmin::OnEndWorld()
	{
		m_State->m_InPlayMode = false;
		entt::registry& registry = GetActiveScene()->m_SceneData.m_Registry;
		ScriptingSystem::OnEnd();
		// Deleting all Rigidbodies
		Physics2DSystem::DeleteBodies(&Box2DWorldSComponent::Get()->World);
		// Resetting Animation / Audio clips
		AudioSystem::OnReset(&AudioEngineSComponent::Get()->Engine);
		AudioEngineSComponent::Get()->Engine.Reset();

#ifdef SMOLENGINE_EDITOR
		Load(GetActiveScene()->m_SceneData.m_filePath);
#endif
	}

	void WorldAdmin::OnUpdate(Frostium::DeltaTime deltaTime)
	{
		entt::registry& registry = GetActiveScene()->m_SceneData.m_Registry;
#ifdef SMOLENGINE_EDITOR
		if (m_State->m_InPlayMode)
		{
			// Updating Phycics
			Physics2DSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());
			// Sending OnProcess callback
			ScriptingSystem::OnTick(deltaTime);
		}
#else
		Box2DPhysicsSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());
		ScriptingSystem::OnTick(registry, deltaTime);
#endif

		// Extracting Camera
		const auto& cameraGroup = registry.view<CameraComponent, TransformComponent>();
		for (const auto& entity : cameraGroup)
		{
			const auto& [camera, transform] = cameraGroup.get<CameraComponent, TransformComponent>(entity);

			// There is no need to render the scene if the camera is not our target or is disabled
			if (!camera.isPrimaryCamera || !camera.isEnabled) { continue; }

			// Calculating ViewProj
			CameraSystem::CalculateView(&camera, &transform);

			// Rendering scene to target framebuffer
			//RenderScene(tuple.Camera.ViewProjectionMatrix, FramebufferSComponent::Get()[0], false, &tuple.Camera, &transform);

			// At the moment we support only one viewport
			break;
		}
	}

	void WorldAdmin::OnEvent(Frostium::Event& e)
	{
		if (!m_State->m_InPlayMode) { return; }
		//UISystem::OnEvent(GetActiveScene()->m_SceneData.m_Registry, e);
	}

	void WorldAdmin::RenderScene(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos, float zNear, float zFar, bool debugDrawEnabled,
		CameraComponent* targetCamera, TransformComponent* cameraTranform)
	{
		entt::registry& registry = GetActiveScene()->m_SceneData.m_Registry;
#ifdef SMOLENGINE_EDITOR
		if (m_State->m_InPlayMode)
			Physics2DSystem::UpdateTransforms();
#else
		Box2DPhysicsSystem::UpdateTransfroms(registry);

#endif
		RendererSystem::BeginDraw(view, proj, camPos, zNear, zFar);
		{
			RendererSystem::SubmitMeshes();
			RendererSystem::Submit2DTextures();
			RendererSystem::SubmitLights();
			if (targetCamera != nullptr && cameraTranform != nullptr)
				RendererSystem::SubmitCanvases(targetCamera, cameraTranform);
		}
		RendererSystem::EndDraw();

		if (debugDrawEnabled)
		{

		}
	}

	void WorldAdmin::ReloadAssets()
	{
		Scene* activeScene = GetActiveScene();
		entt::registry& registry = GetActiveScene()->m_SceneData.m_Registry;

		activeScene->UpdateIDSet();
		Reload2DTextures(registry, activeScene->m_SceneData.m_AssetMap);
		ReloadAudioClips(registry, &AudioEngineSComponent::Get()->Engine);
		Reload2DAnimations(registry);
		ReloadCanvases(registry);
		ReloadMeshMaterials(registry, &activeScene->m_SceneData);
		ScriptingSystem::ReloadScripts();
	}

	void WorldAdmin::OnGameViewResize(float width, float height)
	{
		CameraSystem::OnResize(width, height);
	}

	bool WorldAdmin::Save(const std::string& filePath)
	{
		return GetActiveScene()->Save(filePath);
	}

	bool WorldAdmin::Load(const std::string& filePath)
	{
		std::string path = filePath;
		std::ifstream file(path);
		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", path);
			return false;
		}

		// temp
		{
			size_t hash = m_State->m_Hash("DummuTexture");
			auto& it = m_State->m_TexturesMap.find(hash);
			auto dummy = it->second;

			m_State->m_MeshMap.clear();
			m_State->m_TexturesMap.clear();
			Frostium::MaterialLibrary::GetSinglenton()->Reset();

			Frostium::MaterialCreateInfo defMat = {};
			defMat.SetRoughness(1.0f);
			defMat.SetMetalness(0.2f);
			Frostium::MaterialLibrary::GetSinglenton()->Add(&defMat);

			m_State->m_TexturesMap[hash] = dummy;

			m_State->m_SceneMap.clear();
			m_State->m_ActiveSceneID = 0;
		}

		CreateScene(path);

		if (GetActiveScene()->Load(path))
		{
			// Reloading Assets
			ReloadAssets();
			NATIVE_WARN(std::string("Scene loaded successfully"));
			return true;
		}

		return false;
	}

	bool WorldAdmin::SaveCurrentScene()
	{
		if (m_State->m_SceneMap.size() == 0)
			return false;

		SceneData& data = GetActiveScene()->GetSceneData();
		if (Frostium::Utils::IsPathValid(data.m_filePath))
		{
			return Save(data.m_filePath);
		}

		return false;
	}

	bool WorldAdmin::IsInPlayMode()
	{
		return m_State->m_InPlayMode == true;
	}

	bool WorldAdmin::IsTetxureInPool(const std::string& filePath)
	{
		size_t hash = m_State->m_Hash(filePath);
		auto& it = m_State->m_TexturesMap.find(hash);
		if (it != m_State->m_TexturesMap.end())
			return true;

		return false;
	}

	bool WorldAdmin::IsMeshInPool(const std::string& filePath)
	{
		size_t hash = m_State->m_Hash(filePath);
		auto& it = m_State->m_MeshMap.find(hash);
		if (it != m_State->m_MeshMap.end())
			return true;

		return false;
	}

	Ref<Frostium::Mesh> WorldAdmin::AddOrGetMeshFromPool(const std::string& path)
	{
		size_t hash = m_State->m_Hash(path);
		auto& it = m_State->m_MeshMap.find(hash);
		if (it != m_State->m_MeshMap.end())
			return it->second;

		Ref<Frostium::Mesh> mesh = std::make_shared<Frostium::Mesh>();
		m_State->m_MeshMap[hash] = mesh;
		return mesh;
	}

	Ref<Frostium::Texture> WorldAdmin::AddOrGetTextureFromPool(const std::string& path)
	{
		size_t hash = m_State->m_Hash(path);
		auto& it = m_State->m_TexturesMap.find(hash);
		if (it != m_State->m_TexturesMap.end())
			return it->second;

		Ref<Frostium::Texture> texture = std::make_shared<Frostium::Texture>();
		m_State->m_TexturesMap[hash] = texture;
		return texture;
	}


	void WorldAdmin::CreateScene(const std::string& filePath)
	{
		m_State->m_ActiveSceneID++;
		m_State->m_SceneMap[m_State->m_ActiveSceneID].Init(filePath);
		m_State->m_CurrentRegistry = &m_State->m_SceneMap[m_State->m_ActiveSceneID].GetSceneData().m_Registry;
		ReloadAssets();
	}

	Scene* WorldAdmin::GetActiveScene()
	{
		assert(m_State->m_SceneMap.size() > 0);
		assert(m_State->m_ActiveSceneID > 0);

		return &m_State->m_SceneMap[m_State->m_ActiveSceneID];
	}

	void WorldAdmin::Reload2DTextures(entt::registry& registry, const std::unordered_map<std::string, std::string>& assetMap)
	{
		const auto& view = registry.view<Texture2DComponent>();
		view.each([&](Texture2DComponent& texture)
			{
				auto search = assetMap.find(texture.FileName);
				if (search != assetMap.end())
				{
					Frostium::Texture::Create(search->second, texture.Texture.get());
				}
			});
	}

	void WorldAdmin::Reload2DAnimations(entt::registry& registry)
	{

	}

	void WorldAdmin::ReloadAudioClips(entt::registry& registry, AudioEngine* engine)
	{
		const auto& view = registry.view<AudioSourceComponent>();
		view.each([&](AudioSourceComponent& audio)
			{
				// Relaoding Audio Clips
				AudioSystem::ReloadAllClips(audio, engine);
			});
	}

	void WorldAdmin::ReloadCanvases(entt::registry& registry)
	{
		const auto& view = registry.view<CanvasComponent>();
		view.each([&](CanvasComponent& canvas)
			{

			});
	}

	void WorldAdmin::ReloadMeshMaterials(entt::registry& registry, SceneData* data)
	{
		Frostium::MaterialLibrary* instance = Frostium::MaterialLibrary::GetSinglenton();
		instance->Reset();

		// Default Material = ID 0
		Frostium::MaterialCreateInfo materialCI = {};
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

			NATIVE_ERROR("Material {} not found!", path);
		}
	}

	bool WorldAdmin::LoadStaticComponents()
	{
		auto id = m_GlobalRegistry.create();

		// Engines
		m_GlobalRegistry.emplace<AudioEngineSComponent>(id);
		m_GlobalRegistry.emplace<Box2DWorldSComponent>(id);
		// System States
		m_State = &m_GlobalRegistry.emplace<WorldAdminStateSComponent>(id);
		m_GlobalRegistry.emplace<ProjectConfigSComponent>(id);
		m_GlobalRegistry.emplace<ScriptingSystemStateSComponent>(id);
		m_GlobalRegistry.emplace<JobsSystemStateSComponent>(id);

		return true;
	}

	bool WorldAdmin::LoadMeshComponent(MeshComponent* component, const std::string& filePath, bool reset)
	{
		if (reset)
		{
			component->MeshData.clear();
			component->Mesh = nullptr;

			Frostium::Mesh::Create(filePath, component->Mesh.get());
			component->MeshData.resize(component->Mesh->GetSubMeshes().size() + 1);
			for (auto& data : component->MeshData)
				data.MaterialID = 0;

			component->FilePath = filePath;
			return true;
		}

		Frostium::Mesh::Create(filePath, component->Mesh.get());
		for (auto& data : component->MeshData)
			data.MaterialID = 0;

		return true;
	}

	bool WorldAdmin::SetMeshMaterial(MeshComponent* component, Frostium::Mesh* target_mesh, Frostium::MaterialCreateInfo* info, const std::string& material_path)
	{
		int32_t id = Frostium::MaterialLibrary::GetSinglenton()->Add(info);
		if (id == -1)
			return false;

		uint32_t index = 0;
		if (component->Mesh.get() != target_mesh)
		{
			index++;
			for (auto& sub : component->Mesh->GetSubMeshes())
			{
				if (sub == target_mesh)
					break;
				index++;
			}
		}

		std::hash<std::string_view> hash{};
		component->MeshData[index].MaterialPath = material_path;
		component->MeshData[index].MaterialID = id;
		component->MeshData[index].MaterialHash = hash(material_path);
		return true;
	}
}