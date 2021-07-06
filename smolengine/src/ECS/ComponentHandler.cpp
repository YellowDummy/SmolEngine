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
							comp->MeshPtr = mesh;
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
				comp->MeshPtr = mesh;
				comp->ModelPath = filePath;
				comp->eDefaultType = MeshComponent::DefaultMeshType::None;
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
				for (auto& child : comp->MeshPtr->GetChilds())
				{
					index++;
					if (&child == mesh)
						break;
				}
			}

			MaterialCreateInfo matInfo = {};
			if(matInfo.Load(material_path) == true)
			{
				MaterialLibrary* lib = MaterialLibrary::GetSinglenton();
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
		TextureCreateInfo textureCI = {};
		if (textureCI.Load(filePath) == true)
		{
			Ref<Texture> tex = std::make_shared<Texture>();
			Texture::Create(&textureCI, tex.get());

			comp->TexturePath = filePath;
			comp->Texture = tex;
			return true;
		}

		return false;
	}

	bool ComponentHandler::ValidateBody2DComponent(Rigidbody2DComponent* comp, Ref<Actor>& actor)
	{
		if (comp && actor)
		{
			comp->Actor = actor;
			return true;
		}

		return false;
	}

	bool ComponentHandler::ValidateRigidBodyComponent(RigidbodyComponent* comp, Ref<Actor>& actor)
	{
		if (comp && actor)
		{
			comp->CreateInfo.pActor = actor;
			return true;
		}

		return false;
	}

	bool ComponentHandler::ValidateRigidBodyComponent_Script(RigidbodyComponent* comp, Ref<Actor>& actor)
	{
		if (ValidateRigidBodyComponent(comp, actor))
		{
			TransformComponent* transform = actor->GetComponent<TransformComponent>();
			comp->Create(&comp->CreateInfo, transform->WorldPos, transform->Rotation);

			PhysicsSystem::AttachBodyToActiveScene(dynamic_cast<RigidActor*>(comp));
			return true;
		}

		return false;
	}
}