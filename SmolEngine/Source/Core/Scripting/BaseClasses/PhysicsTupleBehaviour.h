#pragma once
#include "Core/Core.h"
#include "Core/Scripting/BaseClasses/BehaviourPrimitive.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	struct PhysicsBaseTuple;

	struct ResourceTuple;

	///
	
	class PhysicsTupleBehaviour: public BehaviourPrimitive
	{
	public:

		/// Methods to implement:
		/// 
		/// void OnBegin(PhysicsBaseTuple& tuple)
		///
		/// void OnProcess(DeltaTime deltaTime, PhysicsBaseTuple& tuple)
		/// 
		/// void OnDestroy()
		/// 
		/// void OnCollisionContact(Actor* actor, bool isTrigger)
		/// 
		/// void OnCollisionExit(Actor* actor, bool isTrigger)
		/// 


		PhysicsTupleBehaviour() = default;

		virtual ~PhysicsTupleBehaviour() = default;

		/// Getters
		
		PhysicsBaseTuple& GetPhysicsBaseTuple();

		ResourceTuple* GetResourceTuple();

		/// Physics

		void AddForce(const PhysicsBaseTuple& tuple, const glm::vec2& force) const;
		 
		void AddForce(const PhysicsBaseTuple& tuple, const glm::vec2& force, const glm::vec2& point) const;

		void AddForce(const glm::vec2& force);

		void AddForce(const glm::vec2& force, const glm::vec2& point);


	private:

		///Friends

		friend class SystemRegistry;

		/// RTTR

		RTTR_ENABLE(BehaviourPrimitive)

		RTTR_REGISTRATION_FRIEND
	};
}