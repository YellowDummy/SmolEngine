#include "Playground2D.h"

#include "Core/Renderer/Renderer2D.h"
#include <imgui/imgui.h>0

#define MAP_DEMO

static const char* s_Map = 
"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWDDDDDDDDDDDDWWWWWWW"
"WWWWWWWWDDDDWWWWWWDDDDWWWWWW"
"WWWWWDDDDWWWWWWWWWWDDDDWWWWW"
"WWWWWWDDDDWWWWDWWWDDDDWWWWWW"
"WWWWWWWWDDDDDDDDDDDDDWWWWWWW"
"WWWWWWWWDDDDDDDDDDDDDWWWWWWW"
"WWWWWWWWDDDDDDDDDDDWWWWWWWWW"
"WWWWWWWWDDDDDDDDDDDDWWWWWWWW"
"WWWWWWWDDDDDDDDDDDDDDWWWWWWW"
"WWWWWWWWDDDDDDDDDDDDWWWWWWWW"
"WWWWWWWWWWWDDDDWDWWWWWWWWWWW"
"WWWWWWWWWWWWWDDWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
;

static uint32_t s_MapWidth = 30, s_MapHeight = 18;

void Playground2D::OnAttach()
{
	m_CameraController->SetZoom(4.0f);

	m_Texture = SmolEngine::Texture2D::Create("Assets/Textures/Background.png");
	m_SheetTexture = SmolEngine::Texture2D::Create("Assets/Textures/RPGpack_sheet_2X.png");

	m_HouseSubTexture = SmolEngine::SubTexture2D::GenerateFromCoods(m_SheetTexture, { 4.0f, 6.0f }, { 128.0f, 128.0f });
	m_FieldSubTexture = SmolEngine::SubTexture2D::GenerateFromCoods(m_SheetTexture, { 1.0f, 11.0f }, { 128.0f, 128.0f });

	m_TextureMap['D'] = SmolEngine::SubTexture2D::GenerateFromCoods(m_SheetTexture, { 6.0f, 11.0f }, { 128.0f, 128.0f });
	m_TextureMap['W'] = SmolEngine::SubTexture2D::GenerateFromCoods(m_SheetTexture, { 11.0f, 11.0f }, { 128.0f, 128.0f });
}

void Playground2D::OnDetach()
{

} 

void Playground2D::OnUpdate(SmolEngine::DeltaTime deltaTime)
{

	SmolEngine::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
	SmolEngine::RendererCommand::Clear();
	SmolEngine::Renderer2D::ResetDataStats();

	SmolEngine::Renderer2D::BeginScene(m_CameraController->GetCamera());

#ifdef MAP_DEMO
	DrawMap();
#else

	SmolEngine::Renderer2D::DrawQuad({ -2.0f, 0.8f, 0.0f }, { 1.0f, 1.0f }, { 1.5f, 0.2f, 0.7f, 1.0f });
	SmolEngine::Renderer2D::DrawQuad({ 2.0f, f2, 0.0f }, { 1.0f, 1.0f }, { 0.5f, 0.9f, 0.1f, 1.0f });
	SmolEngine::Renderer2D::DrawQuad({ -0.5f, -0.8f, 0.0f }, { 1.0f, 1.0f }, m_Texture);

	SmolEngine::Renderer2D::DrawQuadRotated({ -0.5f, 0.8f, 0.0f }, { 1.0f, 1.0f }, f1, { 0.2f, 1.7f, 1.3f, 1.0f });

	SmolEngine::Renderer2D::DrawSpriteSheetTexture({ -2.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, m_HouseSubTexture);
	SmolEngine::Renderer2D::DrawSpriteSheetTexture({ 1.5f, -2.0f, 0.0f }, { 1.0f, 1.0f }, m_FieldSubTexture);

#endif // MAP_DEMO


	SmolEngine::Renderer2D::EndScene();  

	m_CameraController->OnUpdate(deltaTime);
	AnimateQuads(deltaTime);

}

void Playground2D::OnEvent(SmolEngine::Event& event)
{
	m_CameraController->OnEvent(event);
}

void Playground2D::OnImGuiRender()
{

}

void Playground2D::DrawMap()
{

	for (uint32_t y = 0; y < s_MapHeight; y++)
	{
		for (uint32_t x = 0; x < s_MapWidth; x++)
		{
			char tile = s_Map[x + y * s_MapWidth];

			if (m_TextureMap.find(tile) != m_TextureMap.end())
			{
				SmolEngine::Renderer2D::DrawSpriteSheetTexture({ x - (s_MapWidth / 2.0f), (s_MapHeight / 2.0f) - y, 0.5f }, { 1.0f, 1.0f }, m_TextureMap[tile]);
			}
		}
	}
}

void Playground2D::AnimateQuads(SmolEngine::DeltaTime deltaTime)
{
	f1 += 3.0f * deltaTime;

	if (!switchState)
	{
		f2 += 1.0f * deltaTime;

		if (f2 > 1.0f)
		{
			switchState = true;
		}
	}
	else
	{
		if (f2 > -0.8f)
		{
			f2 -= 1.0f * deltaTime;
		}
		else
		{
			switchState = false;
		}
	}

}
