#pragma once
#include "Core/Core.h"
#include "Core/Renderer/SubTexture.h"
#include "Core/Tools.h"

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct Animation2DFrameKey
	{
		Animation2DFrameKey() = default;

		///

		glm::vec4 TextureColor = glm::vec4(1.0f);

		glm::vec2 TextureScale = glm::vec2(1.0f);

		///

		std::string TexturePath = "";

		std::string FileName = "";

		///

		float Speed = 100.0f;

		///

		Ref<Texture2D> Texture = nullptr;

	private:
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(TexturePath, FileName, Speed, TextureColor.r, TextureColor.g, TextureColor.b, TextureColor.a, TextureScale.x, TextureScale.y);
		}
	};

	class Animation2D
	{
	public:

		Animation2D();

		~Animation2D() = default;

		///

		void Play();

		void Update();

		void ResetAllFrames();

		void Reset();

	private:

		std::unordered_map<int, Ref<Animation2DFrameKey>> m_Frames;

		std::string m_ClipName = "";

		ToolTimer m_Timer;

		///

		Ref<Texture2D> m_CurrentTexture = nullptr;

		Ref<Animation2DFrameKey> m_CurrentFrameKey = nullptr;


		bool m_IsActive = false;

		int m_CurrentIndex = 0;

	private:

		friend class cereal::access;
		friend class Scene;
		friend class EditorLayer;
		friend class AnimationPanel;
		friend class Animation2DController;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Frames, m_ClipName);
		}
	};
}