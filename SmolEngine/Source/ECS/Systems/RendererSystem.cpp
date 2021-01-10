#include "stdafx.h"
#include "RendererSystem.h"

#include "ECS/ComponentsCore.h"
#include "Animation/AnimationClip.h"
#include "ECS/Systems/UISystem.h"

#include "Renderer/Renderer2D.h"
#include "Core/Application.h"

namespace SmolEngine
{
	void RendererSystem::SubmitDebugQuad(const glm::vec3& worldPos, const glm::vec2 scale, float rotation, const glm::vec4& color)
	{
		Renderer2D::DebugDrawQuad(worldPos, scale, rotation, color);
	}

	void RendererSystem::SubmitDebugCircle(const glm::vec3& worldPos, float radius, float rotation, const glm::vec4& color)
	{
		Renderer2D::DebugDrawCircle(worldPos, { radius, 0 }, rotation, color);
	}

	void RendererSystem::SubmitDebugLine(const glm::vec3& startPos, const glm::vec3& endPos, const glm::vec4& color)
	{
		Renderer2D::DebugDrawLine(startPos, endPos, color);
	}

	void RendererSystem::BeginDraw(const glm::mat4& viewProjectionMatrix, const float ambientValue, Ref<Framebuffer>& targetFramebuffer)
	{
		Renderer2D::BeginScene(viewProjectionMatrix, ambientValue, targetFramebuffer);
	}

	void RendererSystem::EndDraw()
	{
		Renderer2D::EndScene();
	}

	void RendererSystem::Render2DTextures(entt::registry& registry)
	{
		const auto& group = registry.view<TransformComponent, Texture2DComponent>();
		for (const auto& entity : group)
		{
			auto& [transform, texture2D] = group.get<TransformComponent, Texture2DComponent>(entity);

			CheckLayerIndex(texture2D.LayerIndex);
			if (texture2D.Enabled && texture2D.Texture != nullptr)
			{
				Renderer2D::SubmitSprite(transform.WorldPos, texture2D.LayerIndex,
					transform.Scale, transform.Rotation.x, texture2D.Texture, 1.0f, texture2D.Color);
			}
		}
	}

	void RendererSystem::Render2DLight(entt::registry& registry)
	{
		const auto& group = registry.view<TransformComponent, Light2DSourceComponent>();
		for (const auto& entity : group)
		{
			auto& [transform, light2D] = group.get<TransformComponent, Light2DSourceComponent>(entity);
			if (light2D.isEnabled)
			{
				Renderer2D::SubmitLight2D(transform.WorldPos + glm::vec3(light2D.Position, 0), light2D.Radius, light2D.Color, light2D.Intensity);
			}
		}
	}

	void RendererSystem::Render2DAnimations(entt::registry& registry)
	{
		const auto& group = registry.view<TransformComponent, Animation2DComponent>();
		for (const auto& entity : group)
		{
			auto& [transform, anim] = group.get<TransformComponent, Animation2DComponent>(entity);

			if (anim.CurrentClip)
			{
				const auto frameKey = anim.CurrentClip->m_CurrentFrameKey;
				if (frameKey != nullptr)
				{
					CheckLayerIndex(anim.IndexLayer);
					Renderer2D::SubmitSprite(transform.WorldPos, anim.IndexLayer, transform.Scale, transform.Rotation.x,
						frameKey->Texture, 1.0f, frameKey->TextureColor);
				}
			}
		}
	}

	void RendererSystem::RenderCanvases(entt::registry& registry, CameraComponent* camera, TransformComponent* cameraTransform)
	{
		const auto& group = registry.view<CanvasComponent>();
		for (const auto& entity : group)
		{
			auto& canvas = group.get<CanvasComponent>(entity);
			UISystem::DrawAllElements(canvas, cameraTransform->WorldPos, camera->ZoomLevel);
		}
	}

	void RendererSystem::CheckLayerIndex(int& index)
	{
		if (index > 10)
		{
			index = 10;
		}

		if (index < 0)
		{
			index = 0;
		}
	}

	void RendererSystem::DebugDraw(entt::registry& registry)
	{
		const auto& group = registry.view<TransformComponent, Body2DComponent>();
		for (const auto& entity : group)
		{
			auto& [transform, body2D] = group.get<TransformComponent, Body2DComponent>(entity);
			const auto& body = body2D.Body;

			if (!body2D.ShowShape)
			{
				return;
			}

			switch (body.m_ShapeType)
			{

			case (int)ShapeType::Box:
			{
				Renderer2D::DebugDrawQuad(transform.WorldPos,
					{ body.m_Shape.x, body.m_Shape.y }, transform.Rotation.x);

				break;
			}
			case (int)ShapeType::Cirlce:
			{
				Renderer2D::DebugDrawCircle(
					{ transform.WorldPos.x + body.m_Offset.x, transform.WorldPos.y + body.m_Offset.y,
					1.0f }, { body.m_Radius,  body.m_Radius }, transform.Rotation.x);

				break;
			}
			default:
				break;
			}
		}
	}
}