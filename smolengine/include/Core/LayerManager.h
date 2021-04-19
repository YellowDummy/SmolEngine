#pragma once

#include "Core/Layer.h"
#include <vector>

namespace SmolEngine 
{
	class LayerManager
	{
	public:

		LayerManager();
		~LayerManager();

		void AddLayer(Layer*);
		void AddOverlay(Layer*);
		void PopLayer(Layer*, bool is_overlay = false);

	private:

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:

		std::vector<Layer*>   m_Layers;
		uint32_t              m_LayerInsertsCount;

		friend class Engine;

	};
}

