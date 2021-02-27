#pragma once
#include "Core/Core.h"
#include "Renderer/RendererShared.h"

namespace SmolEngine
{
	class Mesh;
	class Framebuffer;

	struct DebugViewInfo
	{
		bool           bShowOmniCube = false;
		bool           bShowMRT = false;

		uint32_t       mrtAttachmentIndex = 0;
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

		static void SubmitDirectionalLight(const glm::vec3& dir, const glm::vec4& color);

		static void SubmitPointLight(const glm::vec3& pos, const glm::vec4& color, float constant, float linear, float exp);

		// Setters

		static void SetDebugViewParams(DebugViewInfo& info);

		static void SetShadowLightDirection(const glm::vec3& dir);

		static void SetActiveDebugView(bool value);

		static void SetAmbientMixer(float value);

		static void SetExposure(float value);

		static void SetGamma(float value);

		// Getters

		static Ref<Framebuffer> GetFramebuffer();

	private:

		// Helpers

		static bool OnNewLevelLoaded();

		static bool UpdateMaterials();

		static void Reset();

	private:

		static void Flush();

		static void StartNewBacth();

		/// Helpres

		static void InitPBR();

		static void InitPipelines();

		static void InitFramebuffers();

		static glm::mat4 CalculateDepthMVP(const glm::vec3& lightPos);

	private:

		friend class Renderer3DTestLayer; // temp
	};
}
