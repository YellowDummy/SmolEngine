#include "stdafx.h"
#include "Core/LayerManager.h"

namespace SmolEngine 
{
	LayerManager::LayerManager()
		:m_LayerInsertsCount(0),
		m_Layers(0)
	{

	}

	LayerManager::~LayerManager()
	{
		for (Layer* layer : m_Layers) 
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerManager::AddLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertsCount , layer);
		m_LayerInsertsCount++;
		layer->OnAttach();
	}

	void LayerManager::AddOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerManager::PopLayer(Layer* layer, bool is_overlay)
	{
		auto result = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (result != m_Layers.end() && !is_overlay) 
		{
			m_Layers.erase(result);
			m_LayerInsertsCount--;
		}
		else
		{
			m_Layers.erase(result);
		}
	}
}