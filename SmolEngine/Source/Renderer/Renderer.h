#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	class Mesh;
	struct PBRMaterial;

	class Renderer
	{
	public:

		/// Init

		static void Init();

		static void Shutdown();

		/// Main

		static void BeginScene(const glm::mat4& viewProj);

		static void EndScene();

		/// Submit

		static void SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, const Ref<Mesh>& mesh, const PBRMaterial& PBRmaterial);

		/// Events

		static void OnNewLevelLoaded();

	private:

		static void Flush();

		static void StartNewBacth();

		/// Helpres

		static void InitPBR();

		static void InitMainPipeline();

		static void InitSkyBoxPipeline();
	};
}
