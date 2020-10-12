#include "GameLayer.h"

void GameLayer::OnAttach()
{
	m_Scene = SmolEngine::Scene::GetScene();
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(SmolEngine::DeltaTime deltaTime)
{
	m_Scene->OnUpdate(deltaTime);
}

void GameLayer::OnEvent(SmolEngine::Event& event)
{
	m_Scene->OnEvent(event);
}
