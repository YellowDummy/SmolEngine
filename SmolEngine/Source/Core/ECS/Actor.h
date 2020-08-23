#pragma once

#include <../Libraries/entt/entt.hpp>

#include "Core/ECS/Components.h"

#include "Core/Time.h"
#include "Core/SLog.h"

namespace SmolEngine
{
	class Actor
	{
	public:

		Actor() = default;
		Actor(const entt::entity& entity, entt::registry& reg, const std::string& name, const std::string& tag)
			:Entity(entity), Tag(tag), Reg(reg), Name(name), IsDisabled(false)
		{
			AddComponent<TransfromComponent>();
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			//TEMP
			if (HasComponent<T>())
			{
				NATIVE_ERROR("Actor already has component.");
				abort();
			}

			return Reg.emplace<T>(Entity, std::forward<Args>(args)...);
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

	public:
		bool IsDisabled;
		bool showActions = false;

	private:
		entt::entity Entity;
		std::string Name;
		std::string Tag;
		entt::registry& Reg;

	};
}