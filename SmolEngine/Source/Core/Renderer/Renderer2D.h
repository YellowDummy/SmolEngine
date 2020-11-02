#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/SubTexture.h"
#include <glm/glm.hpp>
#include <vector>


namespace SmolEngine
{
	class OrthographicCamera;

	class Framebuffer;

	///

	enum class DrawableType : uint16_t
	{
		None = 0,

		Sprite,

		Quad,

		Text,

		Animation
	};

	///

	struct Drawable
	{
		glm::vec4 Color = glm::vec4(1.0f);

		glm::vec3 Transform = glm::vec3(1.0f);

		glm::vec2 Scale = glm::vec2(1.0f);

		///

		Ref<Texture2D> Texture = nullptr;

		///

		float Rotation = 0.0f;

		float AmbientValue = 1.0f;

		uint32_t Layer = 0;

		///

		DrawableType Type = DrawableType::None;
	};

	///
	
	struct Light2DBuffer
	{
		Light2DBuffer() = default;

		Light2DBuffer(const glm::vec4 color, const glm::vec3 pos, float r, float intensity)
			:
			Color(color), Offset(pos), Radius(r), Intensity(intensity) {}

		///

		glm::vec3 Color = glm::vec3(1.0f);

		glm::vec3 Offset = glm::vec3(1.0f);

		///

		float Radius = 1.0f;

		float Intensity = 1.0f;
	};

	///

	enum class DebugPrimitives
	{
		None = 0,

		Quad,

		Circle
	};

	///

	//TODO: Implement Batch Rendering

	class Renderer2D
	{
	public:

		static void Init();

		static void Shutdown();

		static void BeginScene(const glm::mat4& viewProjectionMatrix, const float ambientValue);


		static void EndScene();

		static void DrawQuadRotated(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color);

		static void DrawSprite(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D> texture,
float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const glm::vec4& color);

		//UI
		static void DrawUIText(const glm::vec3& pos, const glm::vec2& scale, const Ref<Texture2D> texture, const glm::vec4& tintColor = glm::vec4(1.0f));

		//Debug
		static void BeginDebug(Ref<OrthographicCamera> camera);

		static void EndDebug();

		static void DebugDraw(DebugPrimitives type, const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color = glm::vec4(0.121f, 1.0f, 0.058f, 1.0f));

		//Light
		static void DrawLight2D(const glm::vec3& offset, const float radius, const glm::vec4& color, const float lightIntensity);

		//Animation
		static void DrawAnimation2DPreview(Ref<OrthographicCamera> camera, float ambientValue, const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture,
			float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawAnimation2D(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D> texture,
			float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawFrameBuffer(const uint32_t colorAttachmentID);

		struct RendererData2D
		{
			uint32_t DrawCalls;
			uint32_t QuadCount;

			inline uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			inline uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

	private:

		static bool FindShader(std::string& filePath, const std::string& shaderName);
	};
}