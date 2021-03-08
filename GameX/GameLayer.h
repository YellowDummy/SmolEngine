#pragma once
#include "Core/Core.h"
#include "Core/Layer.h"
#include "Core/Engine.h"
#include "ECS/WorldAdmin.h"

class GameLayer : public SmolEngine::Layer
{
public:

	GameLayer() = default;

	void OnAttach() override;

	void OnDetach() override;

	void OnUpdate(SmolEngine::DeltaTime deltaTime) override;

	void OnEvent(SmolEngine::Event& event) override;

private:

	SmolEngine::WorldAdmin* m_Scene = nullptr;
};