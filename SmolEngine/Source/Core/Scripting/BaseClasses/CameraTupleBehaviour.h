#pragma once

#include "Core/Scripting/BaseClasses/BehaviourPrimitive.h"

namespace SmolEngine
{
	struct CameraBaseTuple;

	///

	class CameraTupleBehaviour: public BehaviourPrimitive
	{
	public:

		/// Methods to implement:
		/// 
		/// void OnBegin(CameraBaseTuple& tuple)
		///
		/// void OnProcess(DeltaTime deltTime, CameraBaseTuple& tuple)
		/// 
		/// void OnDestroy()
		/// 
		
		CameraTupleBehaviour() = default;

		virtual ~CameraTupleBehaviour() = default;

		/// Getters
		
		CameraBaseTuple& GetCameraBaseTuple();

		/// Setters


	private:

		///Friends

		friend class SystemRegistry;

		/// RTTR

		RTTR_ENABLE(BehaviourPrimitive)

		RTTR_REGISTRATION_FRIEND
	};
}