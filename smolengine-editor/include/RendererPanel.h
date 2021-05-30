#pragma once

#include "ECS/Systems/RendererSystem.h"

namespace SmolEngine
{
	class RendererPanel
	{
	public:

		void SetActiveDebugDraw(bool val);
		void OnUpdate(bool& open);
		DebugDrawState* GetDebugState();

	private:

		void UpdateStates();

	private:

		DebugDrawState m_DebugDrawState = {};
	};
}