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
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"
#include "ECS/Components/Singletons/GraphicsEngineSComponent.h"

#include <Frostium3D/DeferredRenderer.h>
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
		scene->CalculateRelativePositions();
		SceneStateComponent* sceneState = scene->GetSceneState();

		CameraSystem::OnBeginWorld();
		Physics2DSystem::OnBeginWorld();
		PhysicsSystem::OnBeginWorld();
		AudioSystem::OnBeginWorld();
		ScriptingSystem::OnBeginWorld();

		m_State->m_InPlayMode = true;
	}

	void WorldAdmin::OnBeginFrame()
	{
		// Extracting Camera
		entt::registry& registry = GetActiveScene()->GetRegistry();
		const auto& cameraGroup = registry.view<CameraComponent, TransformComponent>();
		for (const auto& entity : cameraGroup)
		{
			const auto& [camera, transform] = cameraGroup.get<CameraComponent, TransformComponent>(entity);
			if (m_State->m_LevelEditorActive)
			{
				if (!camera.bPrimaryCamera || !camera.bShowPreview) { continue; }
			}
			else
				if (!camera.bPrimaryCamera) { continue; }

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
	}

	void WorldAdmin::OnEndFrame()
	{

	}

	void WorldAdmin::OnEndWorld()
	{
		m_State->m_InPlayMode = false;
		CameraSystem::OnEndWorld();
		ScriptingSystem::OnEndWorld();
		Physics2DSystem::OnEndWorld();
		PhysicsSystem::OnEndWorld();
		AudioSystem::OnBeginWorld();
	}

	void WorldAdmin::OnUpdate(DeltaTime deltaTime)
	{
		if (m_State->m_InPlayMode)
		{
			Scene* scene = GetActiveScene();
			scene->OnUpdate(deltaTime);

			ScriptingSystem::OnUpdate(deltaTime);
			Physics2DSystem::OnUpdate(deltaTime);
			PhysicsSystem::OnUpdate(deltaTime);

			Physics2DSystem::UpdateTransforms();
			PhysicsSystem::UpdateTransforms();
		}

		RendererSystem::BeginSubmit(&m_State->m_SceneInfo);
		RendererSystem::OnUpdate();
		RendererSystem::EndSubmit();
	}

	void WorldAdmin::OnEvent(Event& e)
	{
		if (!m_State->m_InPlayMode) { return; }
	}

	void WorldAdmin::ReloadActiveScene()
	{
		Scene* activeScene = GetActiveScene();
		SceneStateComponent* sceneState = activeScene->GetSceneState();
		entt::registry& registry = activeScene->m_SceneData.m_Registry;

		// Clear mesh map
		m_State->m_MeshMap.clear();

		// Updates renderer states
		DeferredRenderer::SetRendererState(&sceneState->PipelineState.State);

		// Recrates actor's name & id sets
		{
			activeScene->m_State->ActorIDSet.clear();
			activeScene->m_State->ActorNameSet.clear();

			auto& actors = activeScene->m_State->Actors;
			uint32_t actorCount = static_cast<uint32_t>(actors.size());

			for (uint32_t i = 0; i < actorCount; ++i)
			{
				auto actor = actors[i];
				uint32_t actorID = actor->GetID();
				std::string name = actor->GetName();

				activeScene->m_State->ActorIDSet[actorID] = actor.get();
				activeScene->m_State->ActorNameSet[name] = actor.get();
			}
		}

		// Loads Assets
		{
			ReloadActors();
			Reload2DTextures(registry);
			ReloadAudioClips(registry, &AudioEngineSComponent::Get()->Engine);
			Reload2DAnimations(registry);
			ReloadCanvases(registry);
			ReloadMeshes(registry);
			ReloadRigidBodies(registry);
			ReloadScripts(registry);
		}
	}

	bool WorldAdmin::LoadScene(const std::string& filePath, bool is_reload)
	{
		std::string path = filePath;
		std::ifstream file(path);
		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", path);
			return false;
		}

		// Add default materials and reset renderer
		if(is_reload == false)
		{
			MaterialLibrary::GetSinglenton()->Reset();
			DeferredRenderer::ResetStates();

			Scope<MaterialCreateInfo> defMat = std::make_unique<MaterialCreateInfo>();
			defMat->SetRoughness(1.0f);
			defMat->SetMetalness(0.2f);
			MaterialLibrary::GetSinglenton()->Add(defMat.get(), "defaultmaterial");
			DeferredRenderer::UpdateMaterials();
		}

		Scene* current_scene = &m_State->m_Scenes[m_State->m_ActiveSceneID];
		current_scene->Free();

		if (GetActiveScene()->Load(path))
		{
			ReloadActiveScene();
			m_State->m_CurrentRegistry = &current_scene->m_SceneData.m_Registry;
			NATIVE_WARN(std::string("Scene loaded successfully"));
			return true;
		}

		return false;
	}

	bool WorldAdmin::SwapScene(uint32_t index)
	{
		return false; //temp
	}

	void WorldAdmin::SetBeginSceneInfo(BeginSceneInfo* info)
	{
		assert(info != nullptr);
		m_State->m_SceneInfo = *info;
	}

	bool WorldAdmin::SaveCurrentScene()
	{
		SceneStateComponent* sceneState = GetActiveScene()->GetSceneState();
		if (Utils::IsPathValid(sceneState->FilePath))
		{
			return SaveScene(sceneState->FilePath);
		}

		return false;
	}

	bool WorldAdmin::LoadLastSceneState()
	{
		SceneStateComponent* sceneState = GetActiveScene()->GetSceneState();
		return LoadScene(sceneState->FilePath, true);
	}

	bool WorldAdmin::LoadSceneRuntime(const std::string& path)
	{
		if(m_State->m_CurrentRegistry != nullptr)
			OnEndWorld();

		if (LoadScene(path))
		{
			OnBeginWorld();
			return true;
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
		Scene* scene = &m_State->m_Scenes[m_State->m_ActiveSceneID];
		scene->Create(filePath);

		m_State->m_CurrentRegistry = &scene->m_SceneData.m_Registry;
		return true;
	}

	Scene* WorldAdmin::GetActiveScene()
	{
		return &m_State->m_Scenes[m_State->m_ActiveSceneID];
	}

	void WorldAdmin::Reload2DTextures(entt::registry& registry)
	{
		const auto& view = registry.view<Texture2DComponent>();

		JobsSystem::BeginSubmition();
		{
			view.each([&](Texture2DComponent& comp)
			{
				JobsSystem::Schedule([&comp]() 
				{
					comp.Texture = std::make_shared<Texture>();
					Texture::Create(comp.TexturePath, comp.Texture.get());
				});
			});
		}
		JobsSystem::EndSubmition();
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

	void WorldAdmin::ReloadMeshes(entt::registry& registry)
	{
		MaterialLibrary* lib = MaterialLibrary::GetSinglenton();
		const auto& view = registry.view<MeshComponent>();

		// Mesh loading
		JobsSystem::BeginSubmition();
		{
			view.each([&](MeshComponent& component)
			{
				JobsSystem::Schedule([&component]()
				{
					component.Mesh = std::make_shared<Mesh>();
					Mesh::Create(component.ModelPath, component.Mesh.get());
					if (component.Mesh->GetVertexCount() == 0) // not loaded
						component.Mesh = nullptr;
				});

			});
		}
		JobsSystem::EndSubmition();

		// Loads materials if exist
		{
			view.each([&](MeshComponent& component)
			{
				MaterialCreateInfo materialCI = {};

				// Loads materials if exist
				uint32_t count = static_cast<uint32_t>(component.MaterialsData.size());
				for (uint32_t i = 0; i < count; ++i)
				{
					auto& materialData = component.MaterialsData[i];
					const std::string& path = materialData.Path;

					if (path.empty() == false && std::filesystem::exists(path))
					{
						if (lib->Load(path, materialCI))
						{
							uint32_t matID = lib->Add(&materialCI, path);
							materialData.ID = matID;
						}
					}
				}
			});
		}

		DeferredRenderer::UpdateMaterials();
	}

	void WorldAdmin::ReloadRigidBodies(entt::registry& registry)
	{
		const auto& rigid_view = registry.view<RigidbodyComponent>();
		Scene* activeScene = GetActiveScene();

		JobsSystem::BeginSubmition();
		{
			rigid_view.each([&activeScene](RigidbodyComponent& component)
			{
				JobsSystem::Schedule([&component, &activeScene]()
				{
					component.CreateInfo.pActor = activeScene->FindActorByID(component.CreateInfo.ActorID);
				});

			});;
		}
		JobsSystem::EndSubmition();

	}

	void WorldAdmin::ReloadScripts(entt::registry& reg)
	{
		ScriptingSystemStateSComponent* instance = ScriptingSystemStateSComponent::GetSingleton();

		const auto& view = reg.view<BehaviourComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<BehaviourComponent>(entity);
			Actor* actor = WorldAdmin::GetSingleton()->GetActiveScene()->FindActorByID(behaviour.ActorID);
			if (!actor)
			{
				NATIVE_ERROR("ScriptingSystem::ReloadScripts::Actor not found!");
				continue;
			}

			behaviour.Actor = actor;
			for (auto& script : behaviour.Scripts)
			{
				auto& it = instance->MetaMap.find(script.KeyName);
				if (it == instance->MetaMap.end())
				{
					NATIVE_ERROR("ScriptingSystem::ReloadScripts::Script {} not found!", script.KeyName);
					continue;
				}

				script.Script = it->second.ClassInstance;
				auto& primitive = script.Script.cast<BehaviourPrimitive>();
				primitive.m_Actor = actor;

				auto& s_it = behaviour.OutValues.find(script.KeyName);
				if (s_it == behaviour.OutValues.end())
					continue;

				for (auto& valueInt : s_it->second.Ints)
				{
					for (auto& primitiveValue : primitive.m_OutValues)
					{
						if (primitiveValue.Type == BehaviourPrimitive::OutValueType::Int)
						{
							if (valueInt.Name == primitiveValue.ValueName)
								primitiveValue.Ptr = &valueInt.Value;
						}
					}
				}

				for (auto& valueFloat : s_it->second.Floats)
				{
					for (auto& primitiveValue : primitive.m_OutValues)
					{
						if (primitiveValue.Type == BehaviourPrimitive::OutValueType::Float)
						{
							if (valueFloat.Name == primitiveValue.ValueName)
								primitiveValue.Ptr = &valueFloat.Value;
						}
					}
				}

				for (auto& valueString : s_it->second.Strings)
				{
					for (auto& primitiveValue : primitive.m_OutValues)
					{
						if (primitiveValue.Type == BehaviourPrimitive::OutValueType::String)
						{
							if (valueString.Name == primitiveValue.ValueName)
								primitiveValue.Ptr = &valueString.Value;
						}
					}
				}
			}
		}
	}

	bool WorldAdmin::LoadStaticComponents()
	{
		m_GlobalEntity = m_GlobalRegistry.create();

		// Engines
		m_GlobalRegistry.emplace<AudioEngineSComponent>(m_GlobalEntity);
		Physics2DSystem::m_State = &m_GlobalRegistry.emplace<Box2DWorldSComponent>(m_GlobalEntity);
		PhysicsSystem::m_State = &m_GlobalRegistry.emplace<Bullet3WorldSComponent>(m_GlobalEntity);
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
			state->ActorNameSet.erase(oldName);
			state->ActorNameSet[name] = actor;
			actor->GetInfo()->Name = name;
			return true;
		}

		return false;
	}

	void WorldAdmin::ReloadActors()
	{
		Scene* scene = GetActiveScene();
		for (auto& actor : scene->m_State->Actors)
		{
			HeadComponent* info = actor->GetInfo();

			if (info->ParentID > 0)
				info->Parent = scene->FindActorByID(info->ParentID);

			info->Childs.clear();
			for (auto& id : info->ChildsIDs)
			{
				if (id > 0)
					info->Childs.emplace_back( scene->FindActorByID(id));
			}
		}
	}
}