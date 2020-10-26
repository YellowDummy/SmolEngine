#include "stdafx.h"
#include "RendererSystem.h"

#include "Core/ECS/ComponentTuples/DefaultBaseTuple.h"
#include "Core/ECS/ComponentTuples/PhysicsBaseTuple.h"
#include "Core/ECS/ComponentTuples/CameraBaseTuple.h"
#include "Core/ECS/ComponentTuples/ResourceTuple.h"

#include "Core/Animation/AnimationClip.h"

#include "Core/Renderer/Renderer2D.h"
#include "Core/Application.h"

namespace SmolEngine
{
	void RendererSystem::RenderDefaultTuple(const DefaultBaseTuple& tuple)
	{
		if (tuple.Texture.Enabled && tuple.Texture.Texture != nullptr)
		{
			Renderer2D::DrawSprite(tuple.Transform.WorldPos, tuple.Transform.Scale, tuple.Transform.Rotation.x,
				tuple.Texture.Texture, 1.0f, tuple.Texture.Color);
		}
	}

	void RendererSystem::RenderPhysicsTuple(const PhysicsBaseTuple& tuple)
	{
		if (tuple.Texture.Enabled && tuple.Texture.Texture != nullptr)
		{
			Renderer2D::DrawSprite(tuple.Transform.WorldPos, tuple.Transform.Scale, tuple.Transform.Rotation.x,
				tuple.Texture.Texture, 1.0f, tuple.Texture.Color);
		}
	}

	void RendererSystem::RenderCameraTuple(const CameraBaseTuple& tuple)
	{
		if (!tuple.Camera.isPrimaryCamera) { return; }

		UISystem::DrawAllElements(tuple.Canvas, tuple.Transform.WorldPos, tuple.Camera.ZoomLevel);
	}

	void RendererSystem::RenderAnimation2D(const Animation2DComponent& anim, const TransformComponent& transform)
	{
		if (anim.CurrentClip)
		{
			const auto frameKey = anim.CurrentClip->m_CurrentFrameKey;
			if (frameKey != nullptr)
			{
				Renderer2D::DrawAnimation2D(transform.WorldPos, transform.Scale, transform.Rotation.x, 
					frameKey->Texture, 1.0f, frameKey->TextureColor);
			}
		}
	}

	void RendererSystem::DebugDraw(const PhysicsBaseTuple& tuple)
	{
		const auto& body = tuple.Body;

		if (!body.ShowShape)
		{
			return;
		}

		switch (body.Body.m_ShapeType)
		{

		case (int)ShapeType::Box:
		{
			Renderer2D::DebugDraw(DebugPrimitives::Quad, { tuple.Transform.WorldPos.x, tuple.Transform.WorldPos.y, 1.0f },
				{ body.Body.m_Shape.x * 2 , body.Body.m_Shape.y * 2 }, tuple.Transform.Rotation.x);

			break;
		}
		case (int)ShapeType::Cirlce:
		{
			Renderer2D::DebugDraw(DebugPrimitives::Circle, 
				{ tuple.Transform.WorldPos.x + body.Body.m_Offset.x, tuple.Transform.WorldPos.y + body.Body.m_Offset.y,
				1.0f }, { body.Body.m_Radius,  body.Body.m_Radius }, tuple.Transform.Rotation.x);

			break;
		}
		default:
			break;
		}
	}
}