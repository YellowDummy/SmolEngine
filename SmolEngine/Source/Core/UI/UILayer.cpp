#include "stdafx.h"
#include "UILayer.h"

#include "UICanvas.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Renderer2D.h"

namespace SmolEngine
{
	UILayer* UILayer::s_Instance = new UILayer();

	void UILayer::OnAttach()
	{

	}

	void UILayer::OnUpdate(DeltaTime deltaTime)
	{
		for (const auto canvas : m_ActiveCanvases)
		{
			canvas->OnUpdate();
		}
	}

	void UILayer::OnEvent(Event& event)
	{
		for (const auto canvas: m_ActiveCanvases)
		{
			canvas->OnEvent(event);

			if (m_BlockEvents)
			{
				event.m_Handled = true;
			}
		}
	}

	void UILayer::OnDetach()
	{
		m_ActiveCanvases.clear();
	}

	void UILayer::AddCanvas(Ref<UICanvas> canvas)
	{
		m_ActiveCanvases.push_back(canvas);
	}

	void UILayer::DeleteCanvas(Ref<UICanvas> canvas)
	{
		std::remove(m_ActiveCanvases.begin(), m_ActiveCanvases.end(), canvas);
	}

	void UILayer::Clear()
	{
		m_ActiveCanvases.clear();
	}

	bool UILayer::Contains(Ref<UICanvas> canvas)
	{
		auto result = std::find(m_ActiveCanvases.begin(), m_ActiveCanvases.end(), canvas);

		if (result != m_ActiveCanvases.end())
		{
			return true;
		}

		return false;
	}
}