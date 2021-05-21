#pragma once
#include "Core/Core.h"

#include <string>

namespace Frostium
{
	class Mesh;
}

namespace SmolEngine
{
	struct MeshComponent;
	struct Texture2DComponent;
	struct Body2DComponent;
	struct RigidbodyComponent;
	struct StaticbodyComponent;
	class Actor;

	class ComponentHandler
	{
	public:

		// MeshComponent
		static bool ValidateMeshComponent(MeshComponent* comp, const std::string& filePath);
		static bool SetMeshMaterial(MeshComponent* comp, Frostium::Mesh* mesh, const std::string& material_path);

		//Texture
		static bool ValidateTexture2DComponent(Texture2DComponent* comp, const std::string& filePath);

		//Physics
		static bool ValidateBody2DComponent(Body2DComponent* comp, Actor* actor);
		static bool ValidateRigidBodyComponent(RigidbodyComponent* comp, Actor* actor);
		static bool ValidateRigidBodyComponent_Script(RigidbodyComponent* comp, Actor* actor);
	};
}