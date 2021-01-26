#pragma once
#include "Core/Core.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer2D.h"
#include "Core/AssetManager.h"
#include <glm/glm.hpp>
#include <memory>

namespace SmolEngine
{
	class Shader;

	class Renderer
	{
	public:

		Renderer(const Renderer&) = delete;

		static void Init();

		static void OnWidowResize(uint32_t width, uint32_t height);

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

		inline static void DrawTriangle(const Ref<VertexArray>& vertexArray, uint32_t count = 0, size_t vertices = 0)
		{
			s_RendererAPI->DrawTriangle(vertexArray, count, vertices);
		}

		inline static void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t count = 0, size_t vertices = 0)
		{
			s_RendererAPI->DrawLine(vertexArray, count, vertices);
		}

		inline static void DrawFan(const Ref<VertexArray>& vertexArray, uint32_t count = 0, size_t vertices = 0)
		{
			s_RendererAPI->DrawFan(vertexArray, count, vertices);
		}

		inline static Ref<Shader> LoadShader(const std::string& filePath)
		{
			return AssetManager::GetShaderLib()->Load(filePath);
		}

		inline static Ref<Shader> LoadShader(const std::string& vertexPath, const std::string& fragmentPath, bool optimize, const std::string& computePath)
		{
			return AssetManager::GetShaderLib()->Load(vertexPath, fragmentPath, optimize, computePath);
		}

		inline static Ref<Shader> GetShader(const std::string& shaderName)
		{
			return AssetManager::GetShaderLib()->GetElement(shaderName);
		}

	private:

		static RendererAPI* s_RendererAPI;
	};

}
