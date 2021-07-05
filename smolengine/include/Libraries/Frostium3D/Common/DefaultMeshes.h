#pragma once

#include "Primitives/Mesh.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class DefaultMeshes
	{
	public:
		~DefaultMeshes();

		Mesh* Cube = nullptr;
		Mesh* Sphere = nullptr;
		Mesh* Capsule = nullptr;
		Mesh* Torus = nullptr;

	private:

		void Init(const std::string& root);

		friend class GraphicsContext;
	};
}