#include "stdafx.h"
#include "RendererSystem.h"
#include "Core/Application.h"

#include "ECS/ComponentsCore.h"
#include "ECS/Systems/UISystem.h"
#include "Animation/AnimationClip2D.h"

#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer.h"

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

	void RendererSystem::BeginDraw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos, float zNear, float zFar)
	{
		static BeginSceneInfo info;
		info.farClip = zFar;
		info.nearClip = zNear;
		info.view = view;
		info.proj = proj;
		info.pos = camPos;

		Renderer::BeginScene(info);
		Renderer2D::BeginScene(info);
	}

	void RendererSystem::EndDraw()
	{
		Renderer::EndScene();
		Renderer2D::EndScene();
	}

	void RendererSystem::SubmitLights(entt::registry& registry)
	{
		// Point Lights
		{
			const auto& pGroup = registry.view<TransformComponent, PointLightComponent>();
			for (const auto& entity : pGroup)
			{
				auto& [transform, light] = pGroup.get<TransformComponent, PointLightComponent>(entity);
				if (light.bEnabled)
				{
					Renderer::SubmitPointLight({ transform.WorldPos + light.Offset }, light.Color, light.Constant, light.Linear, light.Exposure);
				}
			}
		}

		// Directional Lights
		{
			bool shadows_casted = false;
			const auto& dView = registry.view<DirectionalLightComponent>();
			for (const auto& entity : dView)
			{
				auto& light = dView.get<DirectionalLightComponent>(entity);
				if (light.bEnabled)
				{
					if (light.bCastShadows && !shadows_casted)
					{
						Renderer::SetShadowLightDirection(light.Direction);
						shadows_casted = true;
					}

					Renderer::SubmitDirectionalLight(light.Direction, light.Color);
				}
			}
		}

		// 2D Point Light
		{
			const auto& group = registry.view<TransformComponent, Light2DSourceComponent>();
			for (const auto& entity : group)
			{
				auto& [transform, light2D] = group.get<TransformComponent, Light2DSourceComponent>(entity);
				if (light2D.IsEnabled)
				{
					Renderer2D::SubmitLight2D(transform.WorldPos + glm::vec3(light2D.Offset, 0), light2D.Radius, light2D.Color, light2D.Intensity);
				}
			}
		}
	}

	void RendererSystem::SubmitMeshes(entt::registry& registry)
	{
		const auto& group = registry.view<TransformComponent, MeshComponent>();
		for (const auto& entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			if (mesh.bShow && mesh.Mesh)
				Renderer::SubmitMesh(transform.WorldPos, transform.Rotation, transform.Scale, mesh.Mesh);
		}
	}

	void RendererSystem::Submit2DTextures(entt::registry& registry)
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

	void RendererSystem::Submit2DAnimations(entt::registry& registry)
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

	void RendererSystem::SubmitCanvases(entt::registry& registry, CameraComponent* camera, TransformComponent* cameraTransform)
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