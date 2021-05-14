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

	void RendererSystem::BeginSubmit(Frostium::BeginSceneInfo* info)
	{
		GraphicsContext::GetSingleton()->UpdateSceneData(info);

		ClearInfo clear = {};
		clear.bClear = true;

		Renderer::BeginScene(&clear);
		clear.bClear = false;
		Renderer2D::BeginScene(&clear);
	}

	void RendererSystem::EndSubmit()
	{
		Renderer::EndScene();
		Renderer2D::EndScene();
	}

	void RendererSystem::Update()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;

		// 3D
		{
			// Meshes
			{
				const auto& group = reg->view<TransformComponent, MeshComponent>();
				for (const auto& entity : group)
				{
					const auto& [transform, mesh_component] = group.get<TransformComponent, MeshComponent>(entity);
					if (mesh_component.bShow && mesh_component.Mesh)
					{
						Frostium::Mesh* mesh = mesh_component.Mesh.get();
						std::vector<Frostium::Mesh>& childs = mesh->GetChilds();
						uint32_t childCount = mesh->GetChildCount();

						Renderer::SubmitMeshEx(transform.WorldPos, transform.Rotation, transform.Scale, mesh, mesh_component.MaterialsData[0].ID);
						for (uint32_t i = 0; i < childCount; i++)
						{
							mesh = &childs[i];
							Renderer::SubmitMeshEx(transform.WorldPos, transform.Rotation, transform.Scale, mesh, mesh_component.MaterialsData[i + 1].ID);
						}
					}
				}
			}

			// Point Lights
			{
				const auto& pGroup = reg->view<TransformComponent, PointLightComponent>();
				for (const auto& entity : pGroup)
				{
					const auto& [transform, comp] = pGroup.get<TransformComponent, PointLightComponent>(entity);

					if (comp.Light.IsActive)
					{
						comp.Light.Position = glm::vec4(transform.WorldPos, 1.0);
						Renderer::SubmitPointLight(&comp.Light);
					}
				}
			}

			// Directional Lights
			{
				bool shadows_casted = false;
				const auto& dView = reg->view<DirectionalLightComponent>();
				for (const auto& entity : dView)
				{
					auto& component = dView.get<DirectionalLightComponent>(entity);
					if(component.Light.IsActive)
						Renderer::SubmitDirLight(&component.Light);
				}
			}
		}
		
		//2D
		{
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
							transform.Scale, transform.Rotation, texture2D.LayerIndex, texture2D.Texture.get(), texture2D.Color);
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
						transform.WorldPos = +glm::vec3(light2D.Offset, 0);
						Renderer2D::SubmitLight2D(transform.WorldPos, light2D.Color, light2D.Radius, light2D.Intensity);
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

	}

	void RendererSystem::DebugDraw()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;

		const auto& group = reg->view<TransformComponent, Body2DComponent>();
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

			case (int)Shape2DType::Box:
			{
				break;
			}
			case (int)Shape2DType::Cirlce:
			{
				break;
			}
			default:
				break;
			}
		}
	}
}