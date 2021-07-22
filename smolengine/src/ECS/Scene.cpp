#include "stdafx.h"
#include "Core/CommandUtils.h"
#include "ECS/Scene.h"
#include "ECS/Actor.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Systems/JobsSystem.h"

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

	void Scene::CalculateRelativePositions() // Note: recursion is not supported
	{
		for (auto& parent : m_State->Actors)
		{
			for (auto child : parent->GetChilds())
			{
				TransformComponent* childT = child->GetComponent<TransformComponent>();
				TransformComponent* parentT = parent->GetComponent<TransformComponent>();
				childT->RelativePos = parentT->WorldPos - childT->WorldPos;
			}
		}
	}

	void Scene::UpdateChildsPositions() // Note: recursion is not supported
	{
		for (auto& parent : m_State->Actors)
		{
			for (auto child : parent->GetChilds())
			{
				if (child != nullptr)
				{
					TransformComponent* childT = child->GetComponent<TransformComponent>();
					TransformComponent* parentT = parent->GetComponent<TransformComponent>();

					childT->WorldPos = parentT->WorldPos - childT->RelativePos;
				}
			}
		}
	}

	Ref<Actor> Scene::CreateActor(const std::string& name, const std::string& tag)
	{
		const auto searchNameResult = m_State->ActorNameSet.find(name);
		if (searchNameResult != m_State->ActorNameSet.end())
		{
			NATIVE_ERROR("[Scene]: Actor {} already exist!", name);
			return nullptr;
		}

		Ref<Actor> actor = std::make_shared<Actor>();
		actor->m_Entity = m_SceneData.m_Registry.create();
		uint32_t id = m_State->LastActorID;

		// Add Head
		HeadComponent& head = m_SceneData.m_Registry.emplace<HeadComponent>(actor->m_Entity);
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
		m_State->Actors.emplace_back(actor);
		return actor;
	}

	Ref<Actor> Scene::FindActorByName(const std::string& name)
	{
		const auto& it = m_State->ActorNameSet.find(name);
		if (it != m_State->ActorNameSet.end())
			return it->second;

		return nullptr;
	}

	Ref<Actor> Scene::FindActorByTag(const std::string& tag)
	{
		const auto& it = std::find_if(m_State->Actors.begin(), m_State->Actors.end(), 
			[&](const Ref<Actor>& another) {return tag == another->GetTag(); });

		if (it != m_State->Actors.end()) { return *it; }
		else { return nullptr; }
	}

	Ref<Actor> Scene::FindActorByID(uint32_t id)
	{
		auto& it = m_State->ActorIDSet.find(id);
		if (it != m_State->ActorIDSet.end())
			return it->second;

		return nullptr;
	}

	void Scene::GetActors(std::vector<Ref<Actor>>& outList)
	{
		uint32_t count = static_cast<uint32_t>(m_State->Actors.size());

		outList.resize(count);
		for(uint32_t i = 0; i < count; ++i)
			outList[i] = m_State->Actors[i];
	}

	void Scene::GetActorsByTag(const std::string& tag, std::vector<Ref<Actor>>& outList)
	{
		uint32_t count = static_cast<uint32_t>(m_State->Actors.size());
		for (uint32_t i = 0; i < count; ++i)
		{
			auto actor = m_State->Actors[i];
			if (tag == actor->GetTag())
				outList.push_back(actor);
		}
	}

	void Scene::DuplicateActor(Ref<Actor>& actor)
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
			meshNew->MeshPtr = meshOld->MeshPtr;
		}
	}

	void Scene::DeleteActor(Ref<Actor>& actor)
	{
		if (actor != nullptr)
		{
			m_State->ActorNameSet.erase(actor->GetName());
			m_SceneData.m_Registry.destroy(*actor);

			auto& list = m_State->Actors;
			std::vector<Ref<Actor>> tempList = list;

			tempList.erase(std::remove_if(tempList.begin(), tempList.end(), [&](Ref<Actor>& elem) { return elem == actor; }), tempList.end());
			list = tempList;
			actor = nullptr;
		}
	}

	void Scene::OnUpdate(DeltaTime delta)
	{
		UpdateChildsPositions();
	}

	bool Scene::Save(const std::string& filePath)
	{
		std::stringstream storageRegistry;
		// Serializing all components and states
		{
			cereal::YAMLOutputArchive output{ storageRegistry };
			entt::snapshot{ m_SceneData.m_Registry }.entities(output).component<
				HeadComponent, CameraComponent,
				ScriptComponent,
				Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Rigidbody2DComponent, MeshComponent,
				PointLightComponent, SpotLightComponent, SceneStateComponent, RendererStateComponent, RigidbodyComponent>(output);
		}

		// Writing result to a file
		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << storageRegistry.str();
			myfile.close();
			m_State->FilePath = filePath;
			return true;
		}

		NATIVE_ERROR("[Scene]: Could not write to a file!");
		return false;
	}

	bool Scene::Load(const std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream buffer;
		if (!file)
		{
			NATIVE_ERROR("[Scene]: Could not open the file: {}", filePath);
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
				ScriptComponent,
				Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Rigidbody2DComponent, MeshComponent,
				PointLightComponent, SpotLightComponent, SceneStateComponent, RendererStateComponent, RigidbodyComponent>(regisrtyInput);
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

	bool Scene::AddCppScript(Ref<Actor>& actor, const std::string& script_name)
	{
		return ScriptingSystem::AttachNativeScript(actor, script_name);
	}

	bool Scene::AddCSharpScript(Ref<Actor>& actor, const std::string& class_name)
	{
		return ScriptingSystem::AttachCSharpScript(actor, class_name);
	}

}