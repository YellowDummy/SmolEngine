#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct PhysicsBaseTuple;

	struct DefaultBaseTuple;

	struct CameraBaseTuple;

	struct Animation2DComponent;

	struct CanvasComponent;

	struct TransformComponent;


	///

	class RendererSystem
	{
	public:

		RendererSystem() = default;

		///

	private:

		// Basic Tuples

		static void RenderDefaultTuple(const DefaultBaseTuple& tuple);

		static void RenderPhysicsTuple(const PhysicsBaseTuple& tuple);

		static void RenderCameraTuple(const CameraBaseTuple& tuple);

		// Animation

		static void RenderAnimation2D(const Animation2DComponent& anim, const TransformComponent& transform);

		/// Debug

		static void DebugDraw(const PhysicsBaseTuple& tuple);

	private:

		friend class EditorLayer;

		friend class Scene;

	};
}