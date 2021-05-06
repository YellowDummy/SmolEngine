#include "stdafx.h"
#include "ECS/ComponentHandler.h"
#include "ECS/Components/MeshComponent.h"
#include "ECS/Components/Texture2DComponent.h"

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
				comp->MaterialPaths.resize(mesh->GetChilds().size() + 1);
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

				mesh->SetMaterialID(id);
				comp->MaterialPaths[index] = material_path;
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
}