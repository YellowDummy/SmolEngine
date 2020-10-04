#pragma once

#include "Core/Core.h"
#include "Core/Layer.h"

#include<vector>

namespace SmolEngine
{
	struct DeltaTime;
	class Event;
	class UICanvas;

	class UILayer : public Layer
	{
	public:

		UILayer() = default;

		void OnAttach() override;

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnDetach() override;

		void AddCanvas(Ref<UICanvas> canvas);

		void DeleteCanvas(Ref<UICanvas> canvas);

		void Clear();

		bool Contains(Ref<UICanvas> canvas);

		static UILayer* GetSingleton() { return s_Instance; }

	private:

		std::vector<Ref<UICanvas>> m_ActiveCanvases;
		bool m_BlockEvents = false;

		static UILayer* s_Instance;
	};
}