#pragma once

#include "Core/Core.h"
#include <glm/glm.hpp>

namespace SmolEngine
{
	class Mesh;
	struct MaterialCreateInfo;
	struct MeshComponent;

	class CommandSystem
	{
	public:

		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& out_translation, glm::vec3& out_rotation, glm::vec3& out_scale);

		static bool ComposeTransform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale, bool is3D, glm::mat4& out_transform);

		static bool LoadMeshComponent(MeshComponent* component, const std::string& filePath, bool reset = true);

		static bool SetMeshMaterial(MeshComponent* component, Mesh* target_mesh, MaterialCreateInfo* info, const std::string& material_path);
	};
}