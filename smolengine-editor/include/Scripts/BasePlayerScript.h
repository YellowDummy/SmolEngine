#pragma once

#include "Scripting/BehaviourPrimitive.h"

namespace SmolEngine
{
	class BasePlayerScript: public BehaviourPrimitive
	{
	public:

		BasePlayerScript();

		void OnProcess(float deltaTime);

		void OnBegin();
		void OnDestroy();
		void OnCollisionContact();
		void OnCollisionExit();

	private:

		float*     m_Speed = nullptr;
		int32_t*   m_Level = nullptr;
	};
}