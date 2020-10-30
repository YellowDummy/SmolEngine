#pragma once

#include <../Libraries/entt/entt.hpp>
#include "Core/Time.h"
#include "Core/SLog.h"
#include "Core/ECS/ComponentTuples/BaseTuple.h"

#include <functional>
#include <vector>
#include <unordered_map>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct OutValue;

	struct DefaultBaseTuple;

	struct PhysicsBaseTuple;

	struct CameraBaseTuple;

	struct ResourceTuple;

	struct HeadComponent;

	///

	enum class ActorBaseType : uint16_t
	{
		DefaultBase,

		PhysicsBase,

		CameraBase
	};

	///

	class Actor
	{
	public:

		Actor();

		Actor(const ActorBaseType baseType, entt::entity entity, size_t index);

		/// Operators

		operator entt::entity() const { return Entity; }

		/// Getters

		std::vector<Ref<Actor>>& GetChilds() { return Childs; }

		Ref<Actor> GetChildByName(const std::string& name);

		Ref<Actor> GetChildByTag(const std::string& tag);

		Ref<Actor> GetParent() { return Parent; }

		const std::string& GetName() const;

		const std::string& GetTag() const;

		const size_t GetID() const;

		/// Setters

		void SetParent(Ref<Actor> parent) { Parent = parent; }

		/// Casting

		DefaultBaseTuple* GetDefaultBaseTuple() const;

		PhysicsBaseTuple* GetPhysicsBaseTuple() const;

		CameraBaseTuple*  GetCameraBaseTuple() const;

		const HeadComponent* GetInfo() const;

	private:

		ActorBaseType ActorType = ActorBaseType::DefaultBase;

		///

		entt::entity Entity;

		///

		std::vector<Ref<Actor>> Childs;

		///

		Ref<Actor> Parent = nullptr;

		///

		size_t Index = 0;

		bool m_showComponentUI = false;

	private:

		friend class cereal::access;

		friend struct ScriptableObject;

		friend class WorldAdmin;

		friend class EditorLayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Entity, ActorType, Parent, Childs, Index);
		}

	};
}