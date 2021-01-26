#include "stdafx.h"
#include "Scene.h"

#include "ECS/Actor.h"
#include "ECS/ComponentsCore.h"
#include "Scripting/SystemRegistry.h"

namespace SmolEngine
{
	void Scene::Init(const std::string& filePath)
	{
		std::filesystem::path p(filePath);
		m_SceneData = SceneData(filePath, p.filename().string());
		LoadSingletons();
	}

	Ref<Actor> Scene::CreateActor(const ActorBaseType baseType, const std::string& name, const std::string& tag)
	{
		if (m_SceneData.m_ID == 0)
		{
			NATIVE_ERROR("The scene is not initialized! Use CreateScene() to initialize the scene");
			abort();
		}

		// Checking if actor already exists
		const auto searchNameResult = m_IDSet.find(name);
		if (searchNameResult != m_IDSet.end())
		{
			NATIVE_ERROR("Actor {} already exist!", name);
			return 0;
		}

		// Generating ID
		auto actorEntity = m_SceneData.m_Registry.create();
		uint32_t id = (uint32_t)actorEntity;

		// Creating Actor
		auto& actorRef = std::make_shared<Actor>(baseType, actorEntity, m_SceneData.m_ActorPool.size());

		switch (baseType)
		{
		case ActorBaseType::DefaultBase:
		{
			auto ref = AddTuple<DefaultBaseTuple>(*actorRef.get());
			ref->Info.ID = id;
			ref->Info.Name = name;
			ref->Info.Tag = tag;

			break;
		}
		case ActorBaseType::CameraBase:
		{
			auto ref = AddTuple<CameraBaseTuple>(*actorRef.get());
			ref->Info.ID = id;
			ref->Info.Name = name;
			ref->Info.Tag = tag;

			break;
		}
		default:
			break;
		}

		AddComponent<TransformComponent>(*actorRef.get());
		m_IDSet[name] = id;
		m_SceneData.m_ActorPool[id] = actorRef;

		return actorRef;
	}

	Ref<Actor> Scene::FindActorByName(const std::string& name)
	{
		auto& result = m_IDSet.find(name);
		if (result == m_IDSet.end())
		{
			return nullptr;
		}

		return FindActorByID(result->second);
	}

	Ref<Actor> Scene::FindActorByTag(const std::string& tag)
	{
		return nullptr;
	}

	Ref<Actor> Scene::FindActorByID(const uint32_t id)
	{
		auto& result = m_SceneData.m_ActorPool.find(id);
		if (result != m_SceneData.m_ActorPool.end())
		{
			return result->second;
		}

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
		bool result_id = m_IDSet.erase(actor->GetName());
		bool result_pool = m_SceneData.m_ActorPool.erase(actor->GetID());

		m_SceneData.m_Registry.remove_if_exists<DefaultBaseTuple>(*actor);
		m_SceneData.m_Registry.remove_if_exists<CameraBaseTuple>(*actor);

		m_SceneData.m_Registry.remove_if_exists<Body2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Texture2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<TransformComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<BehaviourComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Animation2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Light2DSourceComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<CanvasComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<AudioSourceComponent>(*actor);
		actor = nullptr;
	}

	bool Scene::Save(const std::string& filePath)
	{
		std::stringstream storageRegistry;
		std::stringstream storageSceneData;

		// Serializing all Tuples
		{
			cereal::JSONOutputArchive output{ storageRegistry };
			entt::snapshot{ m_SceneData.m_Registry }.entities(output).component<

				DefaultBaseTuple, CameraBaseTuple,

				BehaviourComponent, Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Body2DComponent>(output);
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

			sceneDataInput(m_SceneData.m_ActorPool,
				m_SceneData.m_AssetMap,
				m_SceneData.m_Entity, m_SceneData.m_Gravity.x,
				m_SceneData.m_Gravity.y, m_SceneData.m_ID,
				m_SceneData.m_filePath, m_SceneData.m_fileName,
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

				DefaultBaseTuple, CameraBaseTuple,

				BehaviourComponent, Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Body2DComponent>(regisrtyInput);
		}

		// Loading Singletons-Components
		LoadSingletons();

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
			const auto& view = m_SceneData.m_Registry.view<DefaultBaseTuple>();
			view.each([&](DefaultBaseTuple& tuple)
				{
					m_IDSet[tuple.Info.Name] = tuple.Info.ID;
				});
		}
		{
			const auto& view = m_SceneData.m_Registry.view<CameraBaseTuple>();
			view.each([&](CameraBaseTuple& tuple)
				{
					m_IDSet[tuple.Info.Name] = tuple.Info.ID;
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

	std::vector<Ref<Actor>> Scene::GetActorList()
	{
		std::vector<Ref<Actor>> temp;
		return temp;
	}

	std::vector<Ref<Actor>> Scene::GetActorListByTag(const std::string& tag)
	{
		std::vector<Ref<Actor>> temp;
		return temp;
	}

	std::vector<Ref<Actor>> Scene::GetSortedActorList()
	{
		std::vector<Ref<Actor>> temp;
		for (uint32_t i = 0; i < m_SceneData.m_ActorPool.size(); ++i)
		{
			for (const auto& pair : m_SceneData.m_ActorPool)
			{
				auto [key, actor] = pair;

				if (actor->m_Index == i)
				{
					temp.push_back(actor);
				}
			}
		}

		return temp;
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