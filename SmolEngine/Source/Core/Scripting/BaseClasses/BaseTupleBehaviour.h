#pragma once
#include "Core/Core.h"
#include "Core/Scripting/BaseClasses/BehaviourPrimitive.h"

namespace SmolEngine
{
	struct DefaultBaseTuple;

	struct ResourceTuple;

	class Texture;

	///

	class BaseTupleBehaviour: public BehaviourPrimitive
	{
	public:

		/// Methods to implement:
		/// 
		/// void OnBegin(DefaultBaseTuple& tuple)
		///
		/// void OnProcess(DeltaTime deltTime, DefaultBaseTuple& tuple)
		/// 
		/// void OnDestroy()
		/// 
		
		/// Getters

		DefaultBaseTuple& GetDefaultBaseTuple();

		ResourceTuple* GetResourceTuple();
		
		/// Setters

		void SetTexture(Ref<Texture> texture);

	private:

		///Friends

		friend class SystemRegistry;

		/// RTTR

		RTTR_ENABLE(BehaviourPrimitive)

		RTTR_REGISTRATION_FRIEND
	};
}