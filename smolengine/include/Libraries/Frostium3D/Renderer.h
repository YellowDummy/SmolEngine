#pragma once
#include "Common/Core.h"
#include "Common/RendererShared.h"
#include "Common/RendererStorage.h"

namespace Frostium
{
	class Mesh;
	class Framebuffer;
	struct MeshComponent;
	enum class ShadowMapSize : uint16_t;

	class Renderer
	{
	public:

		static void Init(RendererStorage* storage);
		static void Shutdown();
		static void BeginScene(const ClearInfo* clearInfo);
		static void EndScene();

		static void SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale, Mesh* mesh, const uint32_t& PBRmaterialID = 0);
		static void SubmitDirLight(DirectionalLight* light);
		static void SubmitPointLight(PointLight* light);
		static void SubmitSpotLight(SpotLight* light);

		static void SetRenderingState(RenderingState* state);
		static void SetSceneState(SceneState* state);

		// Getters
		static Framebuffer* GetFramebuffer();
		static uint32_t GetNumObjects();
		// Helpers
		static bool UpdateMaterials();

	private:

		static void OnResize(uint32_t width, uint32_t height);
		static void StartNewBacth();
		static void Reset();
		static void Flush();

		static void InitPBR();
		static void InitPipelines();
		static void InitFramebuffers();
		static void CalculateDepthMVP(glm::mat4& out_mvp);

		static void AddMesh(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, Mesh* mesh, const uint32_t& materialID);

	private:

		friend class GraphicsContext;
	};
}
