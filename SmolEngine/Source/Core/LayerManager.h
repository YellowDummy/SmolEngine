#pragma once

#include "Layer.h"
#include <vector>

namespace SmolEngine 
{
	class SMOL_ENGINE_API LayerManager
	{
	public:
		LayerManager();
		~LayerManager();

		void AddLayer(Layer*);
		void AddOverlay(Layer*);
		void PopLayer(Layer*, bool);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertsCount;

	};
}

