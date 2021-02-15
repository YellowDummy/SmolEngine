#pragma once
#include "Core/Core.h"
#include "Core/Layer.h"
#include "Renderer/Renderer.h"

namespace SmolEngine
{
	class EditorCamera;
	class Mesh;

	class Renderer3DTestLayer : public Layer
	{
	public:

		void OnAttach() override;

		void OnDetach() override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		void OnUpdate(DeltaTime deltaTime) override;

	private:

		Ref<EditorCamera>   m_EditorCamera = nullptr;
		Ref<Mesh>           m_CubeMesh = nullptr;
	};
}