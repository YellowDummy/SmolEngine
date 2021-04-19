#include "stdafx.h"
#include "ECS/Systems/CommandSystem.h"

#include "ECS/ComponentsCore.h"
#include "ECS/WorldAdmin.h"

#include <Frostium3D/Common/Mesh.h>
#include <Frostium3D/MaterialLibrary.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Frostium;

namespace SmolEngine
{
	bool CommandSystem::LoadMeshComponent(MeshComponent* component, const std::string& filePath, bool reset)
	{
		if (reset)
		{
			component->MeshData.clear();
			component->Mesh = nullptr;

			Mesh::Create(filePath, component->Mesh.get());
			component->MeshData.resize(component->Mesh->GetSubMeshes().size() + 1);
			for (auto& data : component->MeshData)
				data.MaterialID = 0;

			component->FilePath = filePath;
			return true;
		}

		Mesh::Create(filePath, component->Mesh.get());
		for (auto& data : component->MeshData)
			data.MaterialID = 0;

		return true;
	}

	bool CommandSystem::SetMeshMaterial(MeshComponent* component, Mesh* target_mesh, MaterialCreateInfo* info, const std::string& material_path)
	{
		int32_t id = MaterialLibrary::GetSinglenton()->Add(info);
		if (id == -1)
			return false;

		uint32_t index = 0;
		if (component->Mesh.get() != target_mesh)
		{
			index++;
			for (auto& sub : component->Mesh->GetSubMeshes())
			{
				if (sub == target_mesh)
					break;
				index++;
			}
		}

		std::hash<std::string_view> hash{};
		component->MeshData[index].MaterialPath = material_path;
		component->MeshData[index].MaterialID = id;
		component->MeshData[index].MaterialHash = hash(material_path);
		return true;
	}
}