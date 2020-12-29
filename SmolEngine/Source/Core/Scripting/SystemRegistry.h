#pragma once
#include "Core/Core.h"
#include <rttr/registration.h>
#include <rttr/type.h>

#include <unordered_map>
#include <string>

namespace SmolEngine
{
	enum class ActorBaseType : uint16_t;

	struct SystemInstance
	{
		rttr::type type = rttr::type::get_by_name("NULL");
		rttr::variant variant;
	};

	class SystemRegistry
	{
	public:

		SystemRegistry() = default;

		///
		
		template<typename T>
		static bool AddDefaultSystem(const std::string& name)
		{
			if (ProcessSystem<T, BaseTupleBehaviour>(name, ActorBaseType::DefaultBase))
			{
				rttr::registration::class_<T>(name.c_str())
					.constructor<>()
					.property("Actor", &T::m_Actor, rttr::registration::private_access)
					.method("OnProcess", &T::OnProcess)
					.method("OnBegin", &T::OnBegin)
					.method("OnDestroy", &T::OnDestroy);

				return true;
			}

			return false;
		}

		

		template<typename T>
		static bool AddCameraSystem(const std::string& name)
		{
			if (ProcessSystem<T, CameraTupleBehaviour>(name, ActorBaseType::CameraBase))
			{
				rttr::registration::class_<T>(name.c_str())
					.constructor<>()
					.property("Actor", &T::m_Actor, rttr::registration::private_access)
					.method("OnProcess", &T::OnProcess)
					.method("OnBegin", &T::OnBegin)
					.method("OnDestroy", &T::OnDestroy);

				return true;
			}

			return false;
		}

		///

		// Don't use! For internal use
		template<typename T, typename T2>
		static bool ProcessSystem(const std::string& name, ActorBaseType type)
		{
			if (SystemRegistry::Get()->Instance == nullptr)
			{
				Instance = new SystemRegistry();
			}

			constexpr bool is_base = std::is_base_of<T2, T>();
			if (!is_base)
			{
				return false;
			}

			auto& systemMap = SystemRegistry::Get()->m_SystemMap;
			systemMap[name] = (uint16_t)type;

			return true;
		}

		static SystemRegistry* Get() { return Instance; }

	private:

		static SystemRegistry* Instance;
		std::unordered_map<std::string, uint16_t> m_SystemMap;

		friend class Application;
		friend class WorldAdmin;
		friend class EditorLayer;
	};


}