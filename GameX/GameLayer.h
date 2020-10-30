#pragma once
#include "Core/Core.h"
#include "Core/Layer.h"
#include "Core/Application.h"
#include "Core/ECS/WorldAdmin.h"

class GameLayer : public SmolEngine::Layer
{
public:

	GameLayer() = default;

	void OnAttach() override;

	void OnDetach() override;

	void OnUpdate(SmolEngine::DeltaTime deltaTime) override;

	void OnEvent(SmolEngine::Event& event) override;

private:

	SmolEngine::Ref<SmolEngine::WorldAdmin> m_Scene = nullptr;
};