#include "stdafx.h"
#include "ECS/Systems/RendererSystem.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Systems/UISystem.h"

#include "Core/Engine.h"

#include <Frostium3D/Renderer.h>
#include <Frostium3D/Renderer2D.h>

using namespace Frostium;

namespace SmolEngine
{
	void RendererSystem::BeginDraw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos, float zNear, float zFar)
	{
		static ClearInfo clearInfo = {};
		clearInfo.bClear = true;
		static BeginSceneInfo info;
		info.farClip = zFar;
		info.nearClip = zNear;
		info.view = view;
		info.proj = proj;
		info.pos = camPos;

		Renderer::BeginScene(&clearInfo, &info);
		clearInfo.bClear = false;
		Renderer2D::BeginScene(&clearInfo, &info);
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
				const auto& [transform, light] = pGroup.get<TransformComponent, PointLightComponent>(entity);
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
				const auto& [transform, light2D] = group.get<TransformComponent, Light2DSourceComponent>(entity);
				if (light2D.IsEnabled)
				{
					Renderer2D::SubmitLight2D(transform.WorldPos + glm::vec3(light2D.Offset, 0), light2D.Color, light2D.Radius, light2D.Intensity);
				}
			}
		}
	}

	void RendererSystem::SubmitMeshes(entt::registry& registry)
	{
		const auto& group = registry.view<TransformComponent, MeshComponent>();
		for (const auto& entity : group)
		{
			const auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			if (mesh.bShow && mesh.Mesh)
				Renderer::SubmitMesh(transform.WorldPos, transform.Rotation, transform.Scale, mesh.Mesh.get());
		}
	}

	void RendererSystem::Submit2DTextures(entt::registry& registry)
	{
		const auto& group = registry.view<TransformComponent, Texture2DComponent>();
		for (const auto& entity : group)
		{
			const auto& [transform, texture2D] = group.get<TransformComponent, Texture2DComponent>(entity);

			CheckLayerIndex(texture2D.LayerIndex);
			if (texture2D.Enabled && texture2D.Texture != nullptr)
			{
				Renderer2D::SubmitSprite(transform.WorldPos,
					transform.Scale, transform.Rotation.x, texture2D.LayerIndex, texture2D.Texture.get(), texture2D.Color);
			}
		}
	}

	void RendererSystem::Submit2DAnimations(entt::registry& registry)
	{

	}

	void RendererSystem::SubmitCanvases(entt::registry& registry, CameraComponent* camera, TransformComponent* cameraTransform)
	{
		const auto& group = registry.view<CanvasComponent>();
		for (const auto& entity : group)
		{
			const auto& canvas = group.get<CanvasComponent>(entity);
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
			const auto& [transform, body2D] = group.get<TransformComponent, Body2DComponent>(entity);
			const auto& body = body2D.Body;

			if (!body2D.ShowShape)
			{
				return;
			}

			switch (body.m_ShapeType)
			{

			case (int)ShapeType::Box:
			{
				break;
			}
			case (int)ShapeType::Cirlce:
			{
				break;
			}
			default:
				break;
			}
		}
	}
}