#pragma once

#include "SmolEngineCore.h"

#include "Renderer/Texture.h"
#include "Renderer/CubeTexture.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/EditorCamera.h"

#include "Renderer/GraphicsPipeline.h"

namespace SmolEngine
{
	class Mesh;

	struct UBOParams
	{
		glm::vec4 lights[4];
		glm::vec4 lightsColors[4];
		float exposure;
		float gamma;
	};

	class PBRTestLayer : public Layer
	{
	public:

		PBRTestLayer()
			:Layer("VulkanTestLayer") {}

		~PBRTestLayer() {}

		/// Main

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		void OnAttach() override;

		void OnDetach() override;


		void BuildTestCommandBuffer();

	private:

		Ref<Texture> m_Tetxure1;
		Ref<Texture> m_Tetxure2;
		Ref<Texture> m_Tetxure3;
		Ref<Texture> m_Tetxure4;
		Ref<Texture> m_Tetxure5;
		Ref<Texture> m_Tetxure6;

		Ref<CubeTexture> m_CubeTexture;

		Ref<Mesh> m_TestMesh = nullptr;

		Ref<GraphicsPipeline> m_Pipeline = nullptr;
		Ref<GraphicsPipeline> m_SkyboxPipeline = nullptr;

		Ref<Framebuffer> m_FrameBuffer;

		glm::vec3 pos = glm::vec3(1.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::vec3 rot = glm::vec3(0.0f);

		Ref<EditorCamera> m_EditorCamera = nullptr;
		glm::vec4 m_AddColor = glm::vec4(1);

		bool m_RorateModel = true;

		UBOParams m_UBOParams = {};
	};
}