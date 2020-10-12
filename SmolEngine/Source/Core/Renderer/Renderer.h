#pragma once
#include "Core/Core.h"
#include "Core/Renderer/RendererAPI.h"
#include "Core/AssetManager.h"
#include "Core/Renderer/Renderer2D.h"
#include <glm/glm.hpp>
#include <memory>

namespace SmolEngine
{
	class OrthographicCamera;
	class Shader;

	class Renderer
	{
	public:

		Renderer(const Renderer&) = delete;

		static void Init();

		static void BeginScene(OrthographicCamera& cameraRef);

		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f));

		static void OnWidowResize(uint32_t width, uint32_t height);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData 
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};

	class RendererCommand
	{
	public:

		inline static void Init()
		{
			s_RendererAPI->Init();

			Renderer2D::Init();
		}

		inline static void BindTexture(uint32_t id)
		{
			s_RendererAPI->BindTexture(id);
		}

		inline static void DisableDepth()
		{
			s_RendererAPI->DisableDepth();
		}

		inline static void Reset()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}

		inline static void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawLine(vertexArray, count);
		}

		inline static void DrawFan(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawFan(vertexArray, count);
		}

		inline static void DrawLight()
		{
			s_RendererAPI->DrawLight();
		}

		inline static Ref<Shader> LoadShader(const std::string& filePath)
		{
			return AssetManager::GetShaderLib()->Load(filePath);
		}

		inline static Ref<Shader> LoadShader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName)
		{
			return AssetManager::GetShaderLib()->Load(vertexSource, fragmentSource, shaderName);
		}

		inline static Ref<Shader> GetShader(const std::string& shaderName)
		{
			return AssetManager::GetShaderLib()->GetElement(shaderName);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};

}
