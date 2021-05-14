#include "stdafx.h"
#include "ECS/ComponentHandler.h"
#include "ECS/ComponentsCore.h"

#include <Frostium3D/Renderer.h>
#include <Frostium3D/MaterialLibrary.h>

namespace SmolEngine
{
	bool ComponentHandler::ValidateMeshComponent(MeshComponent* comp, const std::string& filePath)
	{
		if (comp)
		{
			Ref<Frostium::Mesh> mesh = std::make_shared<Frostium::Mesh>();
			Frostium::Mesh::Create(filePath, mesh.get());
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

	bool ComponentHandler::SetMeshMaterial(MeshComponent* comp, Frostium::Mesh* mesh, const std::string& material_path)
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

			Frostium::MaterialLibrary* lib = Frostium::MaterialLibrary::GetSinglenton();
			Frostium::MaterialCreateInfo matInfo = {};
			if (lib->Load(material_path, matInfo))
			{
				uint32_t id = lib->Add(&matInfo);
				Frostium::Renderer::UpdateMaterials();

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
			Ref<Frostium::Texture> tex = std::make_shared<Frostium::Texture>();
			Frostium::Texture::Create(filePath, tex.get());

			comp->TexturePath = filePath;
			comp->Texture = tex;
			return true;
		}

		return false;
	}

	bool ComponentHandler::ValidateBody2DComponent(Body2DComponent* comp, Actor* actor)
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

	bool ComponentHandler::ValidateStaticBodyComponent(StaticbodyComponent* comp, Actor* actor)
	{
		if (comp && actor)
		{
			comp->CreateInfo.pActor = actor;
			comp->CreateInfo.ActorID = actor->GetID();
			return true;
		}

		return false;
	}
}