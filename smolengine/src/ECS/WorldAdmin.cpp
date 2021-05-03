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
#include "ECS/Components/Singletons/GraphicsEngineSComponent.h"

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
		Scene* scene = GetActiveScene();
		SceneStateComponent* sceneState = scene->GetSceneState();
#ifdef SMOLENGINE_EDITOR
		if (!Frostium::Utils::IsPathValid(sceneState->FilePath))
		{
			NATIVE_ERROR("Path is not valid: {}", sceneState->FilePath);
			return;
		}
		// We save the current scene before starting the simulation
		SaveCurrentScene();
#endif
		Box2DWorldSComponent* world = Box2DWorldSComponent::Get();
		// Setting Box2D Callbacks
		Physics2DSystem::OnBegin(world);
		// Creating rigidbodies and joints
		{
			const auto& view = scene->GetRegistry().view<TransformComponent, Body2DComponent>();
			view.each([&](TransformComponent& tranform, Body2DComponent& body)
			{
					Actor* actor = scene->FindActorByID(body.ActorID);
					Physics2DSystem::CreateBody(&body, &tranform, &world->World, actor);
			});
		}

		// Finding which animation / audio clip should play on awake
		AudioSystem::OnAwake(&AudioEngineSComponent::Get()->Engine);
		// Sending start callback to all enabled scripts
		ScriptingSystem::OnBegin();
		m_State->m_InPlayMode = true;
	}

	void WorldAdmin::OnBeginFrame()
	{
		RendererSystem::BeginSubmit(nullptr); // temp
	}

	void WorldAdmin::OnEndFrame()
	{
		RendererSystem::EndSubmit();
	}

	void WorldAdmin::OnEndWorld()
	{
		SceneStateComponent* sceneState = GetActiveScene()->GetSceneState();
		m_State->m_InPlayMode = false;
		ScriptingSystem::OnEnd();
		// Deleting all Rigidbodies
		Physics2DSystem::DeleteBodies(&Box2DWorldSComponent::Get()->World);
		// Resetting Animation / Audio clips
		AudioSystem::OnReset(&AudioEngineSComponent::Get()->Engine);
		AudioEngineSComponent::Get()->Engine.Reset();

#ifdef SMOLENGINE_EDITOR
		LoadScene(sceneState->FilePath);
#endif
	}

	void WorldAdmin::OnUpdate(Frostium::DeltaTime deltaTime)
	{
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
		entt::registry& registry = GetActiveScene()->GetRegistry();
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
	}

	void WorldAdmin::OnGameViewResize(float width, float height)
	{

	}

	void WorldAdmin::RenderScene(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos, float zNear, float zFar, bool debugDrawEnabled,
		CameraComponent* targetCamera, TransformComponent* cameraTranform)
	{
#ifdef SMOLENGINE_EDITOR
		if (m_State->m_InPlayMode)
			Physics2DSystem::UpdateTransforms();
#else
		Box2DPhysicsSystem::UpdateTransfroms(registry);

#endif
		RendererSystem::Update();
		if (debugDrawEnabled)
		{

		}
	}

	void WorldAdmin::ReloadAssets()
	{
		Scene* activeScene = GetActiveScene();
		SceneStateComponent* sceneState = activeScene->GetSceneState();
		entt::registry& registry = activeScene->m_SceneData.m_Registry;

		Reload2DTextures(registry, sceneState->AssetMap);
		ReloadAudioClips(registry, &AudioEngineSComponent::Get()->Engine);
		Reload2DAnimations(registry);
		ReloadCanvases(registry);
		ReloadMeshMaterials(registry, &activeScene->m_SceneData);
		ScriptingSystem::ReloadScripts();
	}

	bool WorldAdmin::LoadScene(const std::string& filePath)
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
			m_State->m_MeshMap.clear();
			m_State->m_TexturesMap.clear();
			Frostium::MaterialLibrary::GetSinglenton()->Reset();

			Frostium::MaterialCreateInfo defMat = {};
			defMat.SetRoughness(1.0f);
			defMat.SetMetalness(0.2f);
			Frostium::MaterialLibrary::GetSinglenton()->Add(&defMat);
			Frostium::Renderer::UpdateMaterials();

			m_State->m_ActiveSceneID = 0;
		}

		Scene* current_scene = &m_State->m_Scenes[m_State->m_ActiveSceneID];
		current_scene->Free();

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

	bool WorldAdmin::LoadSceneAtIndex(const std::string& filePath, uint32_t index)
	{
		return false;
	}

	bool WorldAdmin::LoadSceneBG(const std::string& filePath)
	{
		return false;
	}

	bool WorldAdmin::SwapScene(uint32_t index)
	{
		return false;
	}

	bool WorldAdmin::SaveCurrentScene()
	{
		SceneStateComponent* sceneState = GetActiveScene()->GetSceneState();
		if (Frostium::Utils::IsPathValid(sceneState->FilePath))
		{
			return SaveScene(sceneState->FilePath);
		}

		return false;
	}

	bool WorldAdmin::SaveScene(const std::string& filePath)
	{
		return GetActiveScene()->Save(filePath);
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


	bool WorldAdmin::CreateScene(const std::string& filePath)
	{
		m_State->m_ActiveSceneID++;
		Scene* scene = &m_State->m_Scenes[m_State->m_ActiveSceneID];
		scene->Create(filePath);
		m_State->m_CurrentRegistry = &scene->m_SceneData.m_Registry;
		ReloadAssets();

		return true; // temp
	}

	Scene* WorldAdmin::GetActiveScene()
	{
		assert(m_State->m_ActiveSceneID > 0);
		return &m_State->m_Scenes[m_State->m_ActiveSceneID];
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
	}

	bool WorldAdmin::LoadStaticComponents()
	{
		m_GlobalEntity = m_GlobalRegistry.create();

		// Engines
		m_GlobalRegistry.emplace<AudioEngineSComponent>(m_GlobalEntity);
		m_GlobalRegistry.emplace<Box2DWorldSComponent>(m_GlobalEntity);
		m_GlobalRegistry.emplace<GraphicsEngineSComponent>(m_GlobalEntity);
		// System States
		m_State = &m_GlobalRegistry.emplace<WorldAdminStateSComponent>(m_GlobalEntity);
		m_GlobalRegistry.emplace<ProjectConfigSComponent>(m_GlobalEntity);
		m_GlobalRegistry.emplace<ScriptingSystemStateSComponent>(m_GlobalEntity);
		m_GlobalRegistry.emplace<JobsSystemStateSComponent>(m_GlobalEntity);

		return true;
	}

	bool WorldAdmin::ChangeActorName(Actor* actor, const std::string& name)
	{
		SceneStateComponent* state = GetActiveScene()->GetSceneState();
		std::string oldName = actor->GetName();
		auto& it = state->ActorNameSet.find(name);
		if (it == state->ActorNameSet.end())
		{
			state->ActorNameSet[name] = actor;
			return true;
		}

		return false;
	}

	bool WorldAdmin::LoadMeshComponent(MeshComponent* component, const std::string& filePath, bool reset)
	{
		if (reset)
		{
			component->MeshData.clear();
			component->Mesh = nullptr;

			Frostium::Mesh::Create(filePath, component->Mesh.get());
			component->MeshData.resize(component->Mesh->GetMeshes().size() + 1);
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

	bool WorldAdmin::SetMeshMaterial(MeshComponent* component, const Frostium::Mesh* target_mesh, Frostium::MaterialCreateInfo* info, const std::string& material_path)
	{
		int32_t id = Frostium::MaterialLibrary::GetSinglenton()->Add(info);
		if (id == -1)
			return false;

		uint32_t index = 0;
		if (component->Mesh.get() != target_mesh)
		{
			index++;
			for (auto& sub : component->Mesh->GetMeshes())
			{
				if (&sub == target_mesh)
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