#pragma once
#include "SmolEngineCore.h"

#include "Renderer/Texture.h"
#include "Renderer/CubeTexture.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Shared.h"

#include "Renderer/GraphicsPipeline.h"

namespace SmolEngine
{
	class VertexBuffer;

	struct PBRParams
	{
		glm::vec4 lights = glm::vec4(-15.0, -15 * 0.5f, -15, 1.0f);
		glm::vec4 lightColors = glm::vec4(0.2f, 0.5f, 0.2f, 1.0f);
		float exposure = 2.5;
		float gamm = 4;
	};

	struct SkyLights
	{
		glm::vec4 light = glm::vec4(-15.0, -15 * 0.5f, -15, 1.0f);
	};

	struct PointLights
	{
		glm::vec4 light = glm::vec4(-15.0, -15 * 0.5f, -15, 1.0f);
		glm::vec4 color = glm::vec4(0.2f, 0.5f, 0.2f, 1.0f);
		float radius = 2.0f;
	};

	class DeferredRenderingTest : public Layer
	{
	public:

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		void OnAttach() override;

		void OnDetach() override;


		void BuildTestCommandBuffer();

	private:

		Ref<Texture>                     m_Tetxure1 = nullptr;
		Ref<Texture>                     m_Tetxure2 = nullptr;
		Ref<Texture>                     m_Tetxure3 = nullptr;
		Ref<Texture>                     m_Tetxure4 = nullptr;
		Ref<Texture>                     m_Tetxure5 = nullptr;
		Ref<Texture>                     m_Tetxure6 = nullptr;
		Ref<Texture>                     m_SSAONoise = nullptr;

		Ref<Texture>                     m_BrickAlbedro = nullptr;
		Ref<Texture>                     m_BrickNormal = nullptr;
		Ref<Texture>                     m_BrickRoughness = nullptr;

		Ref<CubeTexture>                 m_CubeTexture = nullptr;
		Ref<Mesh>                        m_TestMesh = nullptr;
		Ref<Mesh>                        m_SponzaMesh = nullptr;
		Ref<Mesh>                        m_PlaneMesh = nullptr;
		Ref<Framebuffer>                 m_Framebuffer = nullptr;

		Ref<GraphicsPipeline>            m_Pipeline = nullptr;
		Ref<GraphicsPipeline>            m_SSAOPipeline = nullptr;
		Ref<GraphicsPipeline>            m_SSAOBlurPipeline = nullptr;
		Ref<GraphicsPipeline>            m_SkyboxPipeline = nullptr;
		Ref<GraphicsPipeline>            m_CombinationPipeline = nullptr;
		Ref<GraphicsPipeline>            m_PBRPipeline = nullptr;

		Ref<Framebuffer>                 m_DeferredFrameBuffer = nullptr;
		Ref<Framebuffer>                 m_SkyboxFrameBuffer = nullptr;
		Ref<Framebuffer>                 m_SSAOFrameBuffer = nullptr;
		Ref<Framebuffer>                 m_SSAOBlurFrameBuffer = nullptr;
		Ref<EditorCamera>                m_EditorCamera = nullptr;

		glm::vec3                        m_Pos = glm::vec3(0.0f);
		glm::vec3                        m_Scale = glm::vec3(1.0f);
		glm::vec3                        m_Rot = glm::vec3(0.0f);
		glm::vec4                        m_AddColor = glm::vec4(1);

		bool                             m_RorateModel = true;
		bool                             m_SSAOEnabled = true;
		int                              m_MaterialIndex = 0;
		int                              m_DisplayMode = 0;
		PointLights                      m_PointLights = {};
		SkyLights                        m_SkyLights = {};
		PBRParams                        m_PBRParams = {};

		std::vector<glm::mat4>           m_ModelViews;
		Ref<VertexBuffer>                m_InstanceVB = nullptr;
	};
}