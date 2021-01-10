#pragma once
#include "Core/Core.h"
#include "Core/Tools.h"
#include "Animation/Animation2DFrameKey.h"

#include <string>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct AnimationClip
	{
		AnimationClip();

		std::unordered_map<int, Ref<Animation2DFrameKey>> m_Frames;
		std::string m_ClipName = "";

		ToolTimer m_Timer;
		Ref<Texture2D> m_CurrentTexture = nullptr;
		Ref<Animation2DFrameKey> m_CurrentFrameKey = nullptr;

		int m_CurrentIndex = 0;
		bool m_IsActive = false;
		bool m_IsDefaultClip = false;

	private:

		friend class cereal::access;
		friend class WorldAdmin;
		friend class EditorLayer;
		friend class AnimationPanel;
		friend class Animation2DSystem;
		friend class RendererSystem;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Frames, m_ClipName, m_IsDefaultClip);
		}
	};

}