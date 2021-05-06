#pragma once
#include "Common/Core.h"
#include "Common/RendererShared.h"

namespace Frostium
{
	class Mesh;
	class Framebuffer;
	struct MeshComponent;
	struct RendererState;
	struct RendererStorage;
	struct AnimationProperties;
	enum class ShadowMapSize : uint16_t;

	class Renderer
	{
	public:

		static void Init(RendererStorage* storage);
		static void Shutdown();
		static void BeginScene(const ClearInfo* clearInfo);
		static void EndScene();

		// Submit
		static void SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale, Mesh* mesh, const uint32_t& PBRmaterialID = 0);
		static void SubmitDirectionalLight(const glm::vec3& dir, const glm::vec4& color);
		static void SubmitPointLight(const glm::vec3& pos, const glm::vec4& color, float constant, float linear, float exp);

		// Setters
		static void SetRendererState(RendererState* state);
		static void SetAmbientLighting(const glm::vec3& diffuseColor, glm::vec3& specularColor,float IBLscale, bool enableIBL, glm::vec3& ambient);
		static void SetShadowLightDirection(const glm::vec3& dir);

		// Getters
		static Framebuffer* GetFramebuffer();
		static uint32_t GetNumObjects();

		// Helpers
		static bool UpdateMaterials();

	private:

		static void OnResize(uint32_t width, uint32_t height);
		static void Reset();

	private:

		static void Flush();
		static void StartNewBacth();

		static void InitPBR();
		static void InitPipelines();
		static void InitFramebuffers();
		static glm::mat4 CalculateDepthMVP(const glm::vec3& lightPos);

		static void AddMesh(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, Mesh* mesh, const uint32_t& materialID);

	private:

		friend class GraphicsContext;
	};
}
