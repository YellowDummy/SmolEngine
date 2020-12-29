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

	enum class ActorBaseType : uint16_t
	{
		DefaultBase,
		CameraBase
	};

	class Actor
	{
	public:

		Actor();

		Actor(const ActorBaseType baseType, entt::entity entity, size_t index);

		/// 
		/// Operators
		/// 

		operator entt::entity() const { return m_Entity; }

		/// 
		/// Getters
		/// 

		std::vector<Ref<Actor>>& GetChilds() { return m_Childs; }

		Ref<Actor> GetChildByName(const std::string& name);

		Ref<Actor> GetChildByTag(const std::string& tag);

		Ref<Actor> GetParent() { return m_Parent; }

		const std::string& GetName() const;

		const std::string& GetTag() const;

		const size_t GetID() const;

		const size_t GetComponentsCount() const;

		/// 
		/// Setters
		/// 

		void SetParent(Ref<Actor> parent) { m_Parent = parent; }

		const HeadComponent* GetInfo() const;

	private:

		std::vector<Ref<Actor>> m_Childs;
		Ref<Actor> m_Parent = nullptr;
		entt::entity m_Entity;
		ActorBaseType m_ActorType = ActorBaseType::DefaultBase;

		size_t m_Index = 0;
		size_t m_ComponentsCount = 0;
		bool m_showComponentUI = false;

	private:

		friend class cereal::access;
		friend struct ScriptableObject;
		friend class WorldAdmin;
		friend class EditorLayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Entity, m_ActorType, m_Parent, m_Childs, m_Index, m_ComponentsCount);
		}

	};
}