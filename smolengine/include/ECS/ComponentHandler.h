#pragma once
#include "Core/Core.h"

#include <string>

namespace SmolEngine
{
	struct MeshComponent;
	struct Texture2DComponent;
	struct Rigidbody2DComponent;
	struct RigidbodyComponent;
	struct StaticbodyComponent;
	class Actor;
	class Mesh;

	class ComponentHandler
	{
	public:

		// MeshComponent
		static bool ValidateMeshComponent(MeshComponent* comp, const std::string& filePath, bool pooling = true);
		static bool SetMeshMaterial(MeshComponent* comp, Mesh* mesh, const std::string& material_path);

		//Texture
		static bool ValidateTexture2DComponent(Texture2DComponent* comp, const std::string& filePath);

		//Physics
		static bool ValidateBody2DComponent(Rigidbody2DComponent* comp, Actor* actor);
		static bool ValidateRigidBodyComponent(RigidbodyComponent* comp, Actor* actor);
		static bool ValidateRigidBodyComponent_Script(RigidbodyComponent* comp, Actor* actor);
	};
}