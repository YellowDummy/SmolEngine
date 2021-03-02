#include "stdafx.h"
#include "Scene.h"

#include "ECS/Actor.h"
#include "ECS/ComponentsCore.h"
#include "Scripting/SystemRegistry.h"

#include "ECS/ComponentTuples/SingletonTuple.h"
#include "ImGui/EditorConsole.h"

namespace SmolEngine
{
	void Scene::Init(const std::string& filePath)
	{
		std::filesystem::path p(filePath);
		m_SceneData = SceneData(filePath, p.filename().string());
		m_SceneData.Prepare();
		LoadSingletons();
	}

	Ref<Actor> Scene::CreateActor(const std::string& name, const std::string& tag)
	{
		Ref<Actor> new_actor = nullptr;
		// Checking if actor already exists
		const auto searchNameResult = m_IDSet.find(name);
		if (searchNameResult != m_IDSet.end())
		{
			NATIVE_ERROR("Actor {} already exist!", name);
			return new_actor;
		}

		// Getting ID
		auto actorEntity = m_SceneData.m_Registry.create();
		uint32_t id = (uint32_t)actorEntity;

		// Creating Actor
		new_actor = std::make_shared<Actor>(actorEntity, m_SceneData.m_ActorPool.size());

		// Add Head
		auto head = AddComponent<HeadComponent>(*new_actor.get());
		head->ID = id;
		head->Name = name;
		head->Tag = tag;

		// Add Transform
		AddComponent<TransformComponent>(*new_actor.get());
		m_IDSet[name] = id;
		m_SceneData.m_ActorPool[id] = new_actor;
		m_SceneData.m_ActorList.push_back(new_actor);

		return new_actor;
	}

	Ref<Actor> Scene::FindActorByName(const std::string& name)
	{
		auto& result = m_IDSet.find(name);
		if (result == m_IDSet.end())
			return nullptr;

		return FindActorByID(result->second);
	}

	Ref<Actor> Scene::FindActorByTag(const std::string& tag)
	{
		Ref<Actor> actor = nullptr;
		for (const auto& obj : m_SceneData.m_ActorList)
		{
			if (obj->GetTag() == tag)
			{
				actor = obj;
				break;
			}
		}

		return actor;
	}

	Ref<Actor> Scene::FindActorByID(const uint32_t id)
	{
		auto& result = m_SceneData.m_ActorPool.find(id);
		if (result != m_SceneData.m_ActorPool.end())
			return result->second;

		return nullptr;
	}

	void Scene::RemoveChild(Ref<Actor>& parent, Ref<Actor>& child)
	{
		child->SetParent(nullptr);
		parent->GetChilds().erase(std::remove(parent->GetChilds().begin(), parent->GetChilds().end(), child), parent->GetChilds().end());
	}

	void Scene::AddChild(Ref<Actor>& parent, Ref<Actor>& child)
	{
		parent->GetChilds().push_back(child);
		child->SetParent(parent);
	}

	void Scene::DuplicateActor(Ref<Actor>& actor)
	{
	}

	void Scene::DeleteActor(Ref<Actor>& actor)
	{
		m_SceneData.m_Registry.remove_if_exists<HeadComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<TransformComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<CameraComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Body2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Texture2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<BehaviourComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Animation2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Light2DSourceComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<CanvasComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<AudioSourceComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<MeshComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<DirectionalLightComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<PointLightComponent>(*actor);

		m_IDSet.erase(actor->GetName());
		m_SceneData.m_ActorPool.erase(actor->GetID());
		std::remove(m_SceneData.m_ActorList.begin(), m_SceneData.m_ActorList.end(), actor);
		actor = nullptr;
	}

	bool Scene::Save(const std::string& filePath)
	{
		std::stringstream storageRegistry;
		std::stringstream storageSceneData;

		// Serializing all Components
		{
			cereal::JSONOutputArchive output{ storageRegistry };
			entt::snapshot{ m_SceneData.m_Registry }.entities(output).component<
				HeadComponent, CameraComponent,
				BehaviourComponent, Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Body2DComponent, MeshComponent, DirectionalLightComponent,
				PointLightComponent>(output);
		}

		// Serializing scene data
		{
			cereal::JSONOutputArchive output{ storageSceneData };
			m_SceneData.serialize(output);
		}

		// Merging two streams
		std::stringstream result;
		result << storageRegistry.str() << "|" << storageSceneData.str();

		// Writing result to a file
		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << result.str();
			myfile.close();
			CONSOLE_WARN(std::string("Scene saved successfully"));
			return true;
		}

		CONSOLE_ERROR(std::string("Could not write to a file!"));
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

		std::string segment;
		std::vector<std::string> seglist;

		// Spliting string into two
		while (std::getline(buffer, segment, '|'))
		{
			seglist.push_back(segment);
		}

		if (seglist.size() != 2)
		{
			return false;
		}

		// Components
		std::stringstream regisrtyStorage;
		regisrtyStorage << seglist.front();

		// Scene data
		std::stringstream sceneDataStorage;
		sceneDataStorage << seglist.back();

		// Deserializing scene data to a new scene object
		{
			cereal::JSONInputArchive sceneDataInput{ sceneDataStorage };

			sceneDataInput(m_SceneData.m_MaterialPaths,
				m_SceneData.m_ActorPool, m_SceneData.m_AssetMap,
				m_SceneData.m_Entity, m_SceneData.m_Gravity.x,
				m_SceneData.m_Gravity.y, m_SceneData.m_ID,
				m_SceneData.m_filePath,
				m_SceneData.m_Name, m_SceneData.m_AmbientStrength);
		}

		// Deleting Singletons-Components
		DeleteSingletons();

		// The registry must be cleared before writing new data
		CleanRegistry();

		// Deserializing components data to an existing registry object
		{
			cereal::JSONInputArchive regisrtyInput{ regisrtyStorage };

			entt::snapshot_loader{ m_SceneData.m_Registry }.entities(regisrtyInput).component<
				HeadComponent, CameraComponent,
				BehaviourComponent, Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Body2DComponent, MeshComponent, DirectionalLightComponent,
				PointLightComponent>(regisrtyInput);
		}

		// Loading Singletons-Components
		LoadSingletons();

		// Updating ActorList
		m_SceneData.m_ActorList.clear();
		m_SceneData.m_ActorList.reserve(m_SceneData.m_ActorPool.size());
		for (const auto& [key, actor] : m_SceneData.m_ActorPool)
		{
			m_SceneData.m_ActorList.push_back(actor);
		}

		// Reloading Assets

		CONSOLE_WARN(std::string("Scene loaded successfully"));
		return true;
	}

	BehaviourComponent* Scene::AddBehaviour(const std::string& systemName, const Ref<Actor>& actor)
	{
		auto& systemMap = SystemRegistry::Get()->m_SystemMap;
		const auto& result = systemMap.find(systemName);
		if (result == systemMap.end())
		{
			NATIVE_ERROR("System <{}> not found!", systemName);
			return nullptr;
		}

		BehaviourComponent* behaviour = nullptr;
		{
			if (!HasComponent<BehaviourComponent>(*actor))
			{
				behaviour = AddComponent<BehaviourComponent>(*actor);
				behaviour->Actor = actor;
				behaviour->ID = actor->GetID();
			}
			else
			{
				behaviour = GetComponent<BehaviourComponent>(*actor);
			}
		}

		int32_t index = static_cast<int32_t>(actor->m_ComponentsCount);
		actor->m_ComponentsCount++;
		ScriptInstance instance = {};
		{
			instance.type = rttr::type::get_by_name(systemName);
			instance.variant = instance.type.create();
		}

		std::vector< OutValue> tempValues;
		// Creating out-variables 
		{
			auto& primitive = instance.variant.get_wrapped_value_non_const<BehaviourPrimitive>();
			primitive.m_Actor = actor;

			for (const auto& pair : primitive.m_OutFloatVariables)
			{
				const auto [varName, varValue] = pair;

				OutValue value = OutValue(varName, *varValue, OutValueType::Float);
				tempValues.push_back(value);
			}

			for (const auto& pair : primitive.m_OutIntVariables)
			{
				const auto [varName, varValue] = pair;

				OutValue value = OutValue(varName, *varValue, OutValueType::Int);
				tempValues.push_back(value);
			}

			for (const auto& pair : primitive.m_OutStringVariables)
			{
				const auto& [varName, varValue] = pair;

				OutValue value = OutValue(varName, *varValue, OutValueType::String);
				strcpy(value.stringBuffer, varValue->data());
				tempValues.push_back(value);
			}
		}

		behaviour->OutValues[systemName].ScriptID = index;
		behaviour->OutValues[systemName].OutValues = std::move(tempValues);
		behaviour->Scripts.push_back(std::move(instance));
		return behaviour;
	}

	bool Scene::OnActorNameChanged(const std::string& lastName, const std::string& newName)
	{
		auto resultNew = m_IDSet.find(newName);
		if (resultNew != m_IDSet.end())
		{
			CONSOLE_ERROR("Actor with name " + newName + " already exist!");
			return false;
		}

		uint32_t id = m_IDSet[lastName];
		if (m_IDSet.erase(lastName) == 1)
		{
			m_IDSet[newName] = id;
			return true;
		}
		return false;
	}

	void Scene::DeleteSingletons()
	{
		if (m_SceneData.m_Registry.valid(m_SceneData.m_Entity))
		{
			m_SceneData.m_Registry.remove_if_exists<SingletonTuple>(m_SceneData.m_Entity);
			return;
		}

		m_SceneData.m_Entity = m_SceneData.m_Registry.create();
		m_SceneData.m_Registry.remove_if_exists<SingletonTuple>(m_SceneData.m_Entity);
	}

	void Scene::LoadSingletons()
	{
		if (m_SceneData.m_Registry.valid(m_SceneData.m_Entity))
		{
			m_SceneData.m_Registry.emplace_or_replace<SingletonTuple>(m_SceneData.m_Entity);
			return;
		}

		m_SceneData.m_Entity = m_SceneData.m_Registry.create();
		m_SceneData.m_Registry.emplace_or_replace<SingletonTuple>(m_SceneData.m_Entity);
	}

	void Scene::UpdateIDSet()
	{
		m_IDSet.clear();
		{
			const auto& view = m_SceneData.m_Registry.view<HeadComponent>();
			view.each([&](HeadComponent& head)
				{
					m_IDSet[head.Name] = head.ID;
				});
		}
	}

	void Scene::CleanRegistry()
	{
		m_SceneData.m_Registry.clear();
	}

	bool Scene::AddAsset(const std::string& fileName, const std::string& filePath)
	{
		const auto& result = m_SceneData.m_AssetMap.find(fileName);
		if (result == m_SceneData.m_AssetMap.end())
		{
			m_SceneData.m_AssetMap[fileName] = filePath;
			return true;
		}

		NATIVE_WARN("AssetMap: File already exists!");
		return false;
	}

	bool Scene::DeleteAsset(const std::string& fileName)
	{
		if (fileName == "")
			return false;

		return m_SceneData.m_AssetMap.erase(fileName);
	}

	const std::unordered_map<std::string, std::string>& Scene::GetAssetMap()
	{
		return m_SceneData.m_AssetMap;
	}

	std::unordered_map<uint32_t, Ref<Actor>>& Scene::GetActorPool()
	{
		return m_SceneData.m_ActorPool;
	}

	void Scene::GetActorList(std::vector<Ref<Actor>>& outList)
	{
		outList = m_SceneData.m_ActorList;
	}

	void Scene::GetSortedActorList(std::vector<Ref<Actor>>& outList)
	{
		outList.reserve(m_SceneData.m_ActorPool.size());
		for (uint32_t i = 0; i < m_SceneData.m_ActorList.size(); ++i)
		{
			for (const auto& actor : m_SceneData.m_ActorList)
			{
				if (actor->m_Index == i)
				{
					outList.push_back(actor);
				}
			}
		}
	}

	void Scene::GetActorListByTag(const std::string& tag, std::vector<Ref<Actor>>& outList)
	{
		for (const auto& actor : m_SceneData.m_ActorList)
		{
			if (actor->GetTag() == tag)
			{
				outList.push_back(actor);
			}
		}
	}


	SceneData& Scene::GetSceneData()
	{
		return m_SceneData;
	}

	std::unordered_map<std::string, uint32_t>& Scene::GetIDSet()
	{
		return m_IDSet;
	}
}