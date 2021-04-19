#pragma once

#include "Core/Core.h"
#include <glm/glm.hpp>

namespace Frostium
{
	class Mesh;
	struct MaterialCreateInfo;
}

namespace SmolEngine
{
	struct MeshComponent;

	class CommandSystem
	{
	public:

		static bool LoadMeshComponent(MeshComponent* component, const std::string& filePath, bool reset = true);
		static bool SetMeshMaterial(MeshComponent* component, Frostium::Mesh* target_mesh, Frostium::MaterialCreateInfo* info, const std::string& material_path);
	};
}