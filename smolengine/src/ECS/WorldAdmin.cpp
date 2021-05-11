#include "stdafx.h"
#include "ECS/WorldAdmin.h"

#include "ECS/ComponentsCore.h"
#include "ECS/Systems/RendererSystem.h"
#include "ECS/Systems/Physics2DSystem.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Systems/AudioSystem.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Systems/JobsSystem.h"

#include "ECS/Components/Singletons/AudioEngineSComponent.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "ECS/Components/Singletons/ProjectConfigSComponent.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"
#include "ECS/Components/Singletons/ScriptingSystemStateSComponent.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/Components/Singletons/PhysXWorldSComponent.h"
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
#ifndef SMOLENGINE_EDITOR
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

			m_State->m_SceneInfo.View = camera.ViewMatrix;
			m_State->m_SceneInfo.Proj = camera.ProjectionMatrix;
			m_State->m_SceneInfo.NearClip = camera.zNear;
			m_State->m_SceneInfo.FarClip = camera.zFar;
			m_State->m_SceneInfo.Pos = transform.WorldPos;

			// At the moment we support only one viewport
			break;
		}
#endif 
		RendererSystem::BeginSubmit(&m_State->m_SceneInfo);
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
			// Updade 2D phycics
			Physics2DSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());
			// Send OnProcess callback
			ScriptingSystem::OnTick(deltaTime);
			// Set transforms
			Physics2DSystem::UpdateTransforms();
		}
#else
		Box2DPhysicsSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());
		ScriptingSystem::OnTick(registry, deltaTime);
		Box2DPhysicsSystem::UpdateTransfroms(registry);
#endif

		RendererSystem::Update();
	}

	void WorldAdmin::OnEvent(Frostium::Event& e)
	{
		if (!m_State->m_InPlayMode) { return; }
	}

	void WorldAdmin::OnGameViewResize(float width, float height)
	{

	}

	void WorldAdmin::ReloadAssets()
	{
		Scene* activeScene = GetActiveScene();
		SceneStateComponent* sceneState = activeScene->GetSceneState();
		entt::registry& registry = activeScene->m_SceneData.m_Registry;

		Reload2DTextures(registry);
		ReloadAudioClips(registry, &AudioEngineSComponent::Get()->Engine);
		Reload2DAnimations(registry);
		ReloadCanvases(registry);
		ReloadMeshes(registry, &activeScene->m_SceneData);

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

	void WorldAdmin::SetBeginSceneInfo(Frostium::BeginSceneInfo* info)
	{
		m_State->m_SceneInfo = *info;
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

	void WorldAdmin::Reload2DTextures(entt::registry& registry)
	{
		const auto& view = registry.view<Texture2DComponent>();
		view.each([&](Texture2DComponent& comp)
			{
				Ref<Frostium::Texture> tex = std::make_shared<Frostium::Texture>();
				Frostium::Texture::Create(comp.TexturePath, tex.get());
				comp.Texture = tex;
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

	void WorldAdmin::ReloadMeshes(entt::registry& registry, SceneData* data)
	{
		Frostium::MaterialLibrary* lib = Frostium::MaterialLibrary::GetSinglenton();
		lib->Reset();

		// Adds default material
		Frostium::MaterialCreateInfo materialCI = {};
		lib->Add(&materialCI);

		const auto& view = registry.view<MeshComponent>();
		view.each([&](MeshComponent& component)
			{
				// Loads mesh
				Ref<Frostium::Mesh> mesh = std::make_shared<Frostium::Mesh>();
				Frostium::Mesh::Create(component.ModelPath, mesh.get());

				// Loads materials if exist
				for (uint32_t i = 0; i < static_cast<uint32_t>(component.MaterialPaths.size()); ++i)
				{
					const std::string& materialPath = component.MaterialPaths[i];
					if (materialPath.empty() == false && std::filesystem::exists(materialPath))
					{
						if (lib->Load(materialPath, materialCI))
						{
							uint32_t matID = lib->Add(&materialCI);
							if (i == 0) // root node
							{
								mesh->SetMaterialID(matID);
								continue;
							}

							auto& childs = mesh->GetChilds();
							childs[i].SetMaterialID(matID);
						}
					}
				}

				component.Mesh = mesh;

			});

		Frostium::Renderer::UpdateMaterials();
	}

	bool WorldAdmin::LoadStaticComponents()
	{
		m_GlobalEntity = m_GlobalRegistry.create();

		// Engines
		m_GlobalRegistry.emplace<AudioEngineSComponent>(m_GlobalEntity);
		m_GlobalRegistry.emplace<Box2DWorldSComponent>(m_GlobalEntity);
		PhysicsSystem::m_State = &m_GlobalRegistry.emplace<PhysXWorldSComponent>(m_GlobalEntity);
		m_GlobalRegistry.emplace<GraphicsEngineSComponent>(m_GlobalEntity);
		// System States
		m_State = &m_GlobalRegistry.emplace<WorldAdminStateSComponent>(m_GlobalEntity);
		ScriptingSystem::m_State = &m_GlobalRegistry.emplace<ScriptingSystemStateSComponent>(m_GlobalEntity);
		JobsSystem::m_State = &m_GlobalRegistry.emplace<JobsSystemStateSComponent>(m_GlobalEntity);
		m_GlobalRegistry.emplace<ProjectConfigSComponent>(m_GlobalEntity);

		PhysicsSystem::m_World = m_State;
		ScriptingSystem::m_World = m_State;
		RendererSystem::m_World = m_State;
		Physics2DSystem::m_World = m_State;
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
			actor->GetInfo()->Name = name;
			return true;
		}

		return false;
	}
}