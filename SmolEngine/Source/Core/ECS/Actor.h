#pragma once

#include <../Libraries/entt/entt.hpp>
#include "Core/ECS/Components.h"
#include "Core/Time.h"
#include "Core/SLog.h"

#include <functional>
#include <vector>

namespace SmolEngine
{
	class Actor
	{
	public:
		Actor(const entt::entity& entity, entt::registry& reg, const std::string& name, const std::string& tag, const size_t id);
		Ref<Actor> operator==(const Ref<Actor> other);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			//TEMP
			if (HasComponent<T>())
			{
				NATIVE_ERROR("Actor already has component.");
				abort();
			}
			auto& component = Reg.emplace<T>(Entity, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			//TEMP
			if (!HasComponent<T>())
			{
				NATIVE_ERROR("Actor does not have component.");
				abort();
			}

			return Reg.get<T>(Entity);
		}

		template<typename T>
		void DeleteComponent()
		{
			if (!HasComponent<T>())
			{
				NATIVE_ERROR("Actor does not have component.");
				abort();
			}

			Reg.remove_if_exists<T>(Entity);
		}

		template<typename T>
		bool HasComponent() { return Reg.has<T>(Entity); }

		std::string& GetName() { return Name; }
		const std::string& GetTag() { return Tag; }
		const size_t GetID() { return ID; }

		Ref<Actor> GetParent() { return Parent; }
		void SetParent(Ref<Actor> parent) { Parent = parent; }

		std::vector<Ref<Actor>>& GetChilds() { return Childs; }

		Ref<Actor> GetChildByName(const std::string& name);
		Ref<Actor> GetChildByTag(const std::string& tag);

	public:
		bool IsDisabled;
		bool showActions = false;

	private:
		std::vector<Ref<Actor>> Childs;
		Ref<Actor> Parent;
		std::string Name;
		std::string Tag;
		entt::entity Entity;
		entt::registry& Reg;
		size_t ID;
	};
}