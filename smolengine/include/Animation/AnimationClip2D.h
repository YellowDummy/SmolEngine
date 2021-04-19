#pragma once
#include "Core/Core.h"
#include "Core/Timer.h"
#include "Animation/Animation2DFrameKey.h"

#include <string>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <Frostium3D/Common/Texture.h>

namespace SmolEngine
{
	struct AnimationClip2D
	{
		AnimationClip2D() = default;

		// Data

		bool                                                 m_bIsActive = false;
		bool                                                 m_bIsDefaultClip = false;
		int                                                  m_CurrentIndex = 0;

		Ref<Frostium::Texture>                               m_CurrentTexture = nullptr;
		Ref<Animation2DFrameKey>                             m_CurrentFrameKey = nullptr;
		Timer                                                m_Timer;

		std::unordered_map<int, Ref<Animation2DFrameKey>>    m_Frames;
		std::string                                          m_ClipName = "";

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
			archive(m_Frames, m_ClipName, m_bIsActive, m_bIsDefaultClip);
		}
	};

}