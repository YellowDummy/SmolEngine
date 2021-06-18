#include "stdafx.h"
#include "Core/CommandUtils.h"
#include "ECS/ComponentHandler.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/Systems/PhysicsSystem.h"

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/DeferredRenderer.h>
#include <Frostium3D/MaterialLibrary.h>

namespace SmolEngine
{
	static std::atomic<bool>* flag = new std::atomic<bool>(false);

	bool ComponentHandler::ValidateMeshComponent(MeshComponent* comp, const std::string& filePath, bool pooling)
	{
		if (comp)
		{
			Ref<Mesh> mesh = nullptr;
			{
				if (pooling)
				{
					WorldAdminStateSComponent* wState = WorldAdminStateSComponent::GetSingleton();
					size_t hash = wState->m_Hash(filePath);

					auto& it = wState->m_MeshMap.find(hash);
					{
						if (it != wState->m_MeshMap.end())
						{
							mesh = it->second;
							uint32_t size = static_cast<uint32_t>(mesh->GetChilds().size()) + 1;
							comp->MaterialsData.resize(size);
							comp->Mesh = mesh;
							comp->ModelPath = filePath;

							return true;
						}
						else
						{
							mesh = std::make_shared<Mesh>();

							{
								while (flag->exchange(true, std::memory_order_relaxed));
								std::atomic_thread_fence(std::memory_order_acquire);

								wState->m_MeshMap[hash] = mesh;

								std::atomic_thread_fence(std::memory_order_release);
								flag->store(false, std::memory_order_relaxed);
							}
						}
					}


				}
				else
					mesh = std::make_shared<Mesh>();
			}

			Mesh::Create(filePath, mesh.get());
			if (mesh->GetVertexCount() > 0)
			{
				uint32_t size = static_cast<uint32_t>(mesh->GetChilds().size()) + 1;
				comp->MaterialsData.resize(size);
				comp->Mesh = mesh;
				comp->ModelPath = filePath;

				return true;
			}
		}

		return false;
	}

	bool ComponentHandler::SetMeshMaterial(MeshComponent* comp, Mesh* mesh, const std::string& material_path)
	{
		if (comp && mesh)
		{
			uint32_t index = 0;
			if (!mesh->IsRootNode())
			{
				for (auto& child : comp->Mesh->GetChilds())
				{
					index++;
					if (&child == mesh)
						break;
				}
			}

			MaterialLibrary* lib = MaterialLibrary::GetSinglenton();
			MaterialCreateInfo matInfo = {};

			if (lib->Load(material_path, matInfo))
			{
				uint32_t id = lib->Add(&matInfo, material_path);
				DeferredRenderer::UpdateMaterials();

				comp->MaterialsData[index].Path = material_path;
				comp->MaterialsData[index].ID = id;
				return true;
			}
		}

		return false;
	}

	bool ComponentHandler::ValidateTexture2DComponent(Texture2DComponent* comp, const std::string& filePath)
	{
		if (comp)
		{
			Ref<Texture> tex = std::make_shared<Texture>();
			Texture::Create(filePath, tex.get());

			comp->TexturePath = filePath;
			comp->Texture = tex;
			return true;
		}

		return false;
	}

	bool ComponentHandler::ValidateBody2DComponent(Rigidbody2DComponent* comp, Actor* actor)
	{
		if (comp && actor)
		{
			comp->Actor = actor;
			comp->ActorID = actor->GetID();
			return true;
		}

		return false;
	}

	bool ComponentHandler::ValidateRigidBodyComponent(RigidbodyComponent* comp, Actor* actor)
	{
		if (comp && actor)
		{
			comp->CreateInfo.pActor = actor;
			comp->CreateInfo.ActorID = actor->GetID();
			return true;
		}

		return false;
	}

	bool ComponentHandler::ValidateRigidBodyComponent_Script(RigidbodyComponent* comp, Actor* actor)
	{
		if (ValidateRigidBodyComponent(comp, actor))
		{
			TransformComponent* transform = actor->GetComponent<TransformComponent>();
			comp->Body.Create(&comp->CreateInfo, transform->WorldPos, transform->Rotation);
			PhysicsSystem::AttachBodyToActiveScene(&comp->Body);
			return true;
		}

		return false;
	}
}