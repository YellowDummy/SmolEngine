#pragma once
#include "SmolEngineCore.h"
#include <unordered_map>

class Playground2D : public SmolEngine::Layer
{
public:
	Playground2D()
		:Layer("Playerground2D"),
		m_CameraController(std::make_shared<SmolEngine::CameraController>(1280.0f / 720.0f))
	{

	}

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(SmolEngine::DeltaTime deltaTime) override;
	void OnEvent(SmolEngine::Event& event) override;
	void OnImGuiRender() override;

	void DrawMap();
	void AnimateQuads(SmolEngine::DeltaTime deltaTime);

private:
	bool switchState = false;
	float f1 = 0, f2 = -0.8f;
	SmolEngine::Ref<SmolEngine::CameraController> m_CameraController;
	SmolEngine::Ref<SmolEngine::Texture2D> m_Texture;
	SmolEngine::Ref<SmolEngine::Texture2D> m_SheetTexture;

	SmolEngine::Ref<SmolEngine::SubTexture2D> m_HouseSubTexture;
	SmolEngine::Ref<SmolEngine::SubTexture2D> m_FieldSubTexture;

	std::unordered_map<char, SmolEngine::Ref<SmolEngine::SubTexture2D>> m_TextureMap;
};

