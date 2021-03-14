#include "stdafx.h"
#include "CommandSystem.h"

#include "ECS/ComponentsCore.h"
#include "ECS/WorldAdmin.h"

#include "Renderer/Mesh.h"
#include "Renderer/MaterialLibrary.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace SmolEngine
{
    bool CommandSystem::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3 = {};

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
    }

    bool CommandSystem::ComposeTransform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale, bool is3D, glm::mat4& out_transform)
    {
		glm::mat4 rot;
		if (is3D)
		{
			rot = rotate(glm::mat4(1.0f), rotation.x, { 1, 0, 0 }) *
				rotate(glm::mat4(1.0f), rotation.y, { 0, 1, 0 }) *
				rotate(glm::mat4(1.0f), rotation.z, { 0, 0, 1 });
		}
		else
		{
			rot = rotate(glm::mat4(1.0f), rotation.x, { 0, 0, 1.0f });
		}

		out_transform = glm::translate(glm::mat4(1.0f), translation) *
			rot * glm::scale(glm::mat4(1.0f), { scale });

        return true;
    }

	bool CommandSystem::LoadMeshComponent(MeshComponent* component, const std::string& filePath, bool reset)
	{
		if (reset)
		{
			component->MeshData.clear();
			component->Mesh = nullptr;

			component->Mesh = Mesh::Create(filePath);
			component->MeshData.resize(component->Mesh->GetSubMeshes().size() + 1);
			for (auto& data : component->MeshData)
				data.MaterialID = 0;

			component->FilePath = filePath;
			return true;
		}

		component->Mesh = Mesh::Create(filePath);
		for (auto& data : component->MeshData)
			data.MaterialID = 0;

		return true;
	}

	bool CommandSystem::SetMeshMaterial(MeshComponent* component, Mesh* target_mesh, MaterialCreateInfo* info, const std::string& material_path)
	{
		int32_t id = MaterialLibrary::GetSinglenton()->Add(info, material_path);
		if (id == -1)
			return false;

		uint32_t index = 0;
		if (component->Mesh.get() != target_mesh)
		{
			index++;
			for (auto& sub : component->Mesh->GetSubMeshes())
			{
				if (sub.get() == target_mesh)
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