#include "stdafx.h"
#include "Scene.h"

#include "ECS/Actor.h"
#include "ECS/ComponentsCore.h"
#include "ImGui/EditorConsole.h"

namespace SmolEngine
{
	void Scene::Init(const std::string& filePath)
	{
		std::filesystem::path p(filePath);
		m_SceneData = SceneData(filePath, p.filename().string());
		m_SceneData.Prepare();
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
		const auto& result = m_IDSet.find(name);
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
		const auto& result = m_SceneData.m_ActorPool.find(id);
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
		auto newObj = CreateActor(actor->GetName() + "_D", actor->GetTag());
		auto newT = newObj->GetComponent<TransformComponent>();
		auto oldT = actor->GetComponent<TransformComponent>();

		newT->Rotation = oldT->Rotation;
		newT->Scale = oldT->Scale;
		newT->WorldPos = oldT->WorldPos;

		auto meshOld = actor->GetComponent<MeshComponent>();
		if (meshOld)
		{
			auto meshNew = newObj->AddComponent<MeshComponent>();
			meshNew->bCastShadows = meshOld->bCastShadows;
			meshNew->bIsStatic = meshOld->bIsStatic;
			meshNew->bShow = meshOld->bShow;
			meshNew->FilePath = meshOld->FilePath;
			meshNew->MaterialNames = meshOld->MaterialNames;
			meshNew->ShadowType = meshOld->ShadowType;
			meshNew->Mesh = meshOld->Mesh;
		}
	}

	void Scene::DeleteActor(Ref<Actor>& actor)
	{
		m_IDSet.erase(actor->GetName());
		m_SceneData.m_ActorPool.erase(actor->GetID());

		std::vector<Ref<Actor>> tempList;
		tempList.reserve(m_SceneData.m_ActorList.size() - 1);
		for (auto& actorRef : m_SceneData.m_ActorList)
		{
			if (actorRef != actor)
				tempList.push_back(actorRef);
		}

		m_SceneData.m_ActorList = std::move(tempList);

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
		m_SceneData.m_Registry.remove_if_exists<AnimatorComponent>(*actor);

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

		// Updating ActorList
		m_SceneData.m_ActorList.clear();
		m_SceneData.m_ActorList.reserve(m_SceneData.m_ActorPool.size());
		for (const auto& [key, actor] : m_SceneData.m_ActorPool)
		{
			m_SceneData.m_ActorList.push_back(actor);
		}

		CONSOLE_WARN(std::string("Scene loaded successfully"));
		return true;
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