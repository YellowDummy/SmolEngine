#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	class Mesh;
	class Framebuffer;

	class Renderer
	{
	public:

		// Init

		static void Init();

		static void Shutdown();

		// Main

		static void BeginScene(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos);

		static void EndScene();

		// Submit

		static void SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, const Ref<Mesh>& mesh);

		// Getters

		static Ref<Framebuffer> GetFramebuffer();

	private:

		// Helpers

		static bool OnNewLevelLoaded();

		static bool UpdateMaterials();

	private:

		static void Flush();

		static void StartNewBacth();

		/// Helpres

		static void InitPBR();

		static void InitPipelines();

		static void InitFramebuffers();

	private:

		friend class Renderer3DTestLayer; // temp
	};
}
