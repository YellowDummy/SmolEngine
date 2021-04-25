#include "stdafx.h"
#include "ECS/Systems/RendererSystem.h"

#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Systems/UISystem.h"

#include "Core/Engine.h"

#include <Frostium3D/Renderer.h>
#include <Frostium3D/Renderer2D.h>

using namespace Frostium;

namespace SmolEngine
{
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

	void RendererSystem::BeginSubmit(const Frostium::BeginSceneInfo* info)
	{
		ClearInfo clear = {};
		Renderer::BeginScene(&clear, info);
		clear.bClear = false;
		Renderer2D::BeginScene(&clear, info);
	}

	void RendererSystem::EndSubmit()
	{
		Renderer::EndScene();
		Renderer2D::EndScene();
	}

	void RendererSystem::Update()
	{
		entt::registry* reg = WorldAdminStateSComponent::GetSingleton()->m_CurrentRegistry;

		// Meshes
		{
			const auto& group = reg->view<TransformComponent, MeshComponent>();
			for (const auto& entity : group)
			{
				const auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
				if (mesh.bShow && mesh.Mesh)
					Renderer::SubmitMesh(transform.WorldPos, transform.Rotation, transform.Scale, mesh.Mesh.get());
			}
		}
		// 2D Textures
		{
			const auto& group = reg->view<TransformComponent, Texture2DComponent>();
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
		// Point Lights
		{
			const auto& pGroup = reg->view<TransformComponent, PointLightComponent>();
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
			const auto& dView = reg->view<DirectionalLightComponent>();
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
			const auto& group = reg->view<TransformComponent, Light2DSourceComponent>();
			for (const auto& entity : group)
			{
				const auto& [transform, light2D] = group.get<TransformComponent, Light2DSourceComponent>(entity);
				if (light2D.IsEnabled)
				{
					Renderer2D::SubmitLight2D(transform.WorldPos + glm::vec3(light2D.Offset, 0), light2D.Color, light2D.Radius, light2D.Intensity);
				}
			}
		}
		// Canvas
		{
			const auto& group = reg->view<CanvasComponent>();
			for (const auto& entity : group)
			{
				const auto& canvas = group.get<CanvasComponent>(entity);
			}
		}

	}

	void RendererSystem::DebugDraw()
	{
		entt::registry& reg = WorldAdmin::GetSingleton()->GetActiveScene()->GetRegistry();

		const auto& group = reg.view<TransformComponent, Body2DComponent>();
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