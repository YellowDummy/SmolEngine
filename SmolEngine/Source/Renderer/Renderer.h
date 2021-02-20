#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	class Mesh;
	class Framebuffer;

	struct BeginSceneInfo
	{
		float          nearClip;
		float          farClip;

		glm::vec3      pos;
		glm::mat4      proj;
		glm::mat4      view;
	};

	class Renderer
	{
	public:

		// Init

		static void Init();

		static void Shutdown();

		// Main

		static void BeginScene(const BeginSceneInfo& info);

		static void EndScene();

		// Submit

		static void SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, const Ref<Mesh>& mesh, int32_t materialID = -1);

		static void SubmitDirectionalLight(const glm::vec3& pos, const glm::vec4& color);

		static void SubmitPointLight(const glm::vec3& pos, const glm::vec4& color, float constant, float linear, float exp);

		// Setters

		static void SetAmbientMixer(float value);

		static void SetGamma(float value);

		static void SetExposure(float value);

		// Getters

		static Ref<Framebuffer> GetFramebuffer();

	private:

		// Helpers

		static bool OnNewLevelLoaded();

		static bool UpdateMaterials();

		static void UpdateCascades();

		static void Reset();

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
