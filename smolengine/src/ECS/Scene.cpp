#include "stdafx.h"
#include "ECS/Scene.h"
#include "ECS/Actor.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Systems/ScriptingSystem.h"

#include <cereal/archives/yaml.hpp>

namespace SmolEngine
{
	Scene::Scene(const Scene& another)
	{

	}

	void Scene::Create(const std::string& filePath)
	{
		std::filesystem::path p(filePath);
		m_SceneData.Init();
		m_State = &m_SceneData.m_Registry.emplace<SceneStateComponent>(m_SceneData.m_Entity);
		m_State->FilePath = filePath;
	}

	void Scene::Free()
	{
		m_State = nullptr;
		m_SceneData.Free();
	}

	Actor* Scene::CreateActor(const std::string& name, const std::string& tag)
	{
		const auto searchNameResult = m_State->ActorNameSet.find(name);
		if (searchNameResult != m_State->ActorNameSet.end())
		{
			NATIVE_ERROR("Actor {} already exist!", name);
			return nullptr;
		}

		auto actorEntity = m_SceneData.m_Registry.create();
		uint32_t id = m_State->LastActorID;
		m_State->Actors.push_back(Actor(actorEntity));
		Actor* actor = &m_State->Actors.back();

		// Add Head
		HeadComponent& head = m_SceneData.m_Registry.emplace<HeadComponent>(*actor);
		head.ComponentID = 0;
		head.ComponentsCount++;
		head.ActorID = id;
		head.Name = name;
		head.Tag = tag;

		// Add Transform
		AddComponent<TransformComponent>(actor);
		m_State->ActorNameSet[name] = actor;
		m_State->ActorIDSet[id] = actor;
		m_State->LastActorID++;
		return actor;
	}

	Actor* Scene::FindActorByName(const std::string& name)
	{
		const auto& it = m_State->ActorNameSet.find(name);
		if (it != m_State->ActorNameSet.end())
			return it->second;

		return nullptr;
	}

	Actor* Scene::FindActorByTag(const std::string& tag)
	{
		for (auto& actor : m_State->Actors)
			if (tag == actor.GetTag())
				return &actor;

		return nullptr;
	}

	Actor* Scene::FindActorByID(uint32_t id)
	{
		auto& it = m_State->ActorIDSet.find(id);
		if (it != m_State->ActorIDSet.end())
			return it->second;

		return nullptr;
	}

	void Scene::GetActors(std::vector<Actor*>& outList)
	{
		uint32_t count = static_cast<uint32_t>(m_State->Actors.size());

		outList.resize(count);
		for(uint32_t i = 0; i < count; ++i)
			outList[i] = &m_State->Actors[i];
	}

	void Scene::GetActorsByID(std::vector<Actor*>& outList)
	{
		uint32_t count = static_cast<uint32_t>(m_State->Actors.size());
		outList.reserve(count);

		for (uint32_t i = 0; i < m_State->LastActorID; ++i)
		{
			for (uint32_t x = 0; x < count; ++x)
			{
				Actor* actor = &m_State->Actors[x];
				if (i == actor->GetID())
					outList.push_back(actor);
			}
		}
	}

	void Scene::GetActorsByTag(const std::string& tag, std::vector<Actor*>& outList)
	{
		uint32_t count = static_cast<uint32_t>(m_State->Actors.size());
		for (uint32_t i = 0; i < count; ++i)
		{
			Actor* actor = &m_State->Actors[i];
			if (tag == actor->GetTag())
				outList.push_back(actor);
		}
	}

	void Scene::RemoveChild(Actor* parent, Actor* child)
	{
		child->SetParent(nullptr);
		parent->GetChilds().erase(std::remove(parent->GetChilds().begin(), parent->GetChilds().end(), child), parent->GetChilds().end());
	}

	void Scene::AddChild(Actor* parent, Actor* child)
	{
		parent->GetChilds().push_back(child);
		child->SetParent(parent);
	}

	void Scene::DuplicateActor(Actor* actor)
	{
		auto newObj = CreateActor(actor->GetName() + "_D", actor->GetTag());
		auto newT = newObj->GetComponent<TransformComponent>();
		auto oldT = actor->GetComponent<TransformComponent>();

		// Transform
		*newT = *oldT;

		if (actor->HasComponent<MeshComponent>())
		{
			auto meshOld = actor->GetComponent<MeshComponent>();
			auto meshNew = newObj->AddComponent<MeshComponent>();

			meshNew->bIsStatic = meshOld->bIsStatic;
			meshNew->bShow = meshOld->bShow;
			meshNew->ModelPath = meshOld->ModelPath;
			meshNew->MaterialsData= meshOld->MaterialsData;
			meshNew->ShadowType = meshOld->ShadowType;
			meshNew->Mesh = meshOld->Mesh;
		}

		if (actor->HasComponent<BehaviourComponent>())
		{
			auto scriptOld = actor->GetComponent<BehaviourComponent>();
			auto scriptNew = newObj->AddComponent<BehaviourComponent>();
			*scriptNew = *scriptOld;
		}

	}

	void Scene::DeleteActor(Actor* actor)
	{
		if (actor != nullptr)
		{
			uint32_t position = actor->GetID();
			m_State->ActorNameSet.erase(actor->GetName());
			m_State->Actors.erase(m_State->Actors.begin() + position);

			m_SceneData.m_Registry.remove_if_exists<HeadComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<TransformComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<CameraComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<Rigidbody2DComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<Texture2DComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<BehaviourComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<Animation2DComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<Light2DSourceComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<CanvasComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<AudioSourceComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<MeshComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<DirectionalLightComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<PointLightComponent>(*actor);
			m_SceneData.m_Registry.remove_if_exists<RigidbodyComponent>(*actor);

			m_SceneData.m_Registry.destroy(*actor);
			actor = nullptr;
		}
	}

	bool Scene::Save(const std::string& filePath)
	{
		std::stringstream storageRegistry;
		// Serializing all components and states
		{
			cereal::YAMLOutputArchive output{ storageRegistry };
			entt::snapshot{ m_SceneData.m_Registry }.entities(output).component<
				HeadComponent, CameraComponent,
				BehaviourComponent, Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Rigidbody2DComponent, MeshComponent, DirectionalLightComponent,
				PointLightComponent, SceneStateComponent, RigidbodyComponent>(output);
		}

		// Writing result to a file
		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << storageRegistry.str();
			myfile.close();
			m_State->FilePath = filePath;
			NATIVE_WARN(std::string("Scene saved successfully"));
			return true;
		}

		NATIVE_ERROR(std::string("Could not write to a file!"));
		return false;
	}

	bool Scene::Load(const std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream buffer;
		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", filePath);
			return false;
		}
		// Copying file content to a buffer
		buffer << file.rdbuf();
		file.close();
		// The registry must be cleared before writing new data
		CleanRegistry();
		// Deserializing components data to an existing registry object
		{
			cereal::YAMLInputArchive regisrtyInput{ buffer };

			entt::snapshot_loader{ m_SceneData.m_Registry }.entities(regisrtyInput).component<
				HeadComponent, CameraComponent,
				BehaviourComponent, Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Rigidbody2DComponent, MeshComponent, DirectionalLightComponent,
				PointLightComponent, SceneStateComponent, RigidbodyComponent>(regisrtyInput);
		}

		// Updates sets
		m_State = GetStateComponent();
		m_State->FilePath = filePath;
		return true;
	}

	SceneStateComponent* Scene::GetStateComponent()
	{
		if (HasComponent<SceneStateComponent>(m_SceneData.m_Entity))
			return &m_SceneData.m_Registry.get<SceneStateComponent>(m_SceneData.m_Entity);

		return nullptr;
	}

	void Scene::CleanRegistry()
	{
		m_SceneData.m_Registry.clear();
	}

	SceneStateComponent* Scene::GetSceneState()
	{
		return m_State;
	}

	entt::registry& Scene::GetRegistry()
	{
		return m_SceneData.m_Registry;
	}

	bool Scene::AddScript(Actor* actor, const std::string& name)
	{
		return ScriptingSystem::AttachNativeScript(actor, name);
	}
}