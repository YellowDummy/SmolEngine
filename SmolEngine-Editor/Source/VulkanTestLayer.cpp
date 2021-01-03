#include "stdafx.h"
#include "VulkanTestLayer.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/Renderer/Vulkan/VulkanPipelineSpecification.h"


namespace SmolEngine
{
	void VulkanTestLayer::OnAttach()
	{
		float aspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();
		m_EditorCamera = std::make_shared<EditorCameraController>(aspectRatio);
		m_EditorCamera->SetZoom(4.0f);

		FramebufferSpecification framebufferCI = {};
		framebufferCI.Width = VulkanContext::GetSwapchain().GetWidth();
		framebufferCI.Height = VulkanContext::GetSwapchain().GetHeight();
		m_FrameBuffer = Framebuffer::Create(framebufferCI);

		struct Vertex
		{
			glm::vec3 Pos;
			glm::vec4 Color;
			glm::vec2 TexCood;
		};

		Vertex verticies[4] =
		{
			// pos                       // color                    // texCood
			{ { -0.5f, 0.5, 0.0f },  { 1.0f, 0.0f, 1.0f, 1.0f },  { 0.0f, 1.0f },},
			{ { 0.5f, 0.5f, 0.0f },  { 0.2f, 1.0f, 0.0f, 1.0f },  { 1.0f, 1.0f},},
			{ {  0.5f, -0.5, 0.0f},  { 0.5f, 0.0f, 1.0f, 1.0f },  { 1.0f, 0.0f },},
			{ { -0.5f, -0.5, 0.0f },  { 1.0f, 0.0f, 1.0f, 1.0f },  { 0.0f, 0.0f } },
		};

		BufferLayout layout({ 

			{ ShaderDataType::Float3, "aPos" }, // location 0
			{ ShaderDataType::Float4, "aColor" },
			{ ShaderDataType::Float2, "aTexCood" } // location 2
		});

		m_Tetxure1 = Texture2D::Create("../GameX/Assets/Textures/SummerBG.png");
		m_Tetxure2 = Texture2D::Create("../GameX/Assets/Textures/bulkhead-wallsx3.png");
		m_Tetxure3 = Texture2D::CreateWhiteTexture();

		VertexBufferCreateInfo vertexBufferCI = {};
		{
			vertexBufferCI.BufferLayot = &layout;
			vertexBufferCI.Sizes = { sizeof(verticies) };
			vertexBufferCI.Vertices = { &verticies };
			vertexBufferCI.Stride = sizeof(Vertex);
		}

		uint32_t indices[6] = { 0, 1, 2,  2, 3, 0 };
		IndexBufferCreateInfo indexBufferCI = {};
		{
			indexBufferCI.IndicesCounts = { 6 };
			indexBufferCI.Indices = { indices };
		}

		GraphicsPipelineShaderCreateInfo shaderCI = {};
		{
			shaderCI.FilePaths[ShaderType::Vertex] = "../SmolEngine/Assets/Shaders/VulkanTriangle_Vertex.glsl";
			shaderCI.FilePaths[ShaderType::Fragment] = "../SmolEngine/Assets/Shaders/VulkanTriangle_Fragment.glsl";
			shaderCI.Textures = {  };
		}

		GraphicsPipelineCreateInfo graphicsPipelineCI = {};
		{
			graphicsPipelineCI.IndexBuffer = &indexBufferCI;
			graphicsPipelineCI.VertexBuffer = &vertexBufferCI;
			graphicsPipelineCI.ShaderCreateInfo = &shaderCI;
		}

		bool result = m_GraphicsPipeline.Create(&graphicsPipelineCI);


	}

	void VulkanTestLayer::OnDetach()
	{

	}

	void VulkanTestLayer::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		BuildTestCommandBuffer();
	}

	void VulkanTestLayer::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);

		if (event.m_EventType == (int)EventType::S_WINDOW_RESIZE)
		{
			auto& e = static_cast<WindowResizeEvent&>(event);
			m_FrameBuffer->OnResize(e.GetWidth(), e.GetHeight());
		}
	}

	void VulkanTestLayer::OnImGuiRender()
	{
		ImGui::Begin("Shader Settings");
		{
			if (ImGui::Button("Reload Shader"))
			{

			}


			ImGui::ColorPicker4("Add Color", glm::value_ptr(m_AddColor));

			ImGui::NewLine();

			ImGui::Image(m_FrameBuffer->GetImGuiTextureID(), ImVec2{ 100, 100 });
		}
		ImGui::End();
	}

	void VulkanTestLayer::BuildTestCommandBuffer()
	{
		m_GraphicsPipeline.BeginCommandBuffer();

		{
			m_GraphicsPipeline.Update2DTextures({ m_Tetxure1 });

			// Fist Render Pass - Main
			{
				uint32_t bindingPoint = 0;
				m_GraphicsPipeline.SumbitUniformBuffer(bindingPoint, sizeof(glm::mat4), &m_EditorCamera->GetCamera()->GetViewProjectionMatrix());

				m_GraphicsPipeline.BeginRenderPass(m_FrameBuffer, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
				{
					m_GraphicsPipeline.SumbitPushConstant(ShaderType::Fragment, sizeof(glm::vec3), &m_AddColor);
					m_GraphicsPipeline.DrawIndexed();
				}
				m_GraphicsPipeline.EndRenderPass();
			}

		}
		m_GraphicsPipeline.EndCommandBuffer();


	}
}