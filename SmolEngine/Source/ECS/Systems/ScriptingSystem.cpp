#include "stdafx.h"
#include "ScriptingSystem.h"

namespace SmolEngine
{
	void ScriptingSystem::OnSceneBegin(entt::registry& registry)
	{
		const auto& view = registry.view<BehaviourComponent>();
		view.each([&](BehaviourComponent& behaviour)
		{
			for (auto& script : behaviour.Scripts)
			{
				script.type.invoke("OnBegin", script.variant, {});
			}
		});
	}

	void ScriptingSystem::OnSceneEnd(entt::registry& registry)
	{
		const auto& view = registry.view<BehaviourComponent>();
		view.each([&](BehaviourComponent& behaviour)
		{
			for (auto& script : behaviour.Scripts)
			{
				script.type.invoke("OnDestroy", script.variant, {});
			}
		});
	}

	void ScriptingSystem::OnSceneTick(entt::registry& registry, DeltaTime deltaTime)
	{
		const auto& view = registry.view<BehaviourComponent>();
		view.each([&](BehaviourComponent& behaviour)
		{
			for (auto& script : behaviour.Scripts)
			{
				script.type.invoke("OnProcess", script.variant, { deltaTime });
			}
		});
	}

	void ScriptingSystem::OnDestroy(Ref<Actor>& actor)
	{
		BehaviourComponent* behaviour = WorldAdmin::GetSingleton()->GetActiveScene().GetComponent<BehaviourComponent>(*actor.get());
		if (behaviour)
		{
			for (auto& script : behaviour->Scripts)
			{
				script.type.invoke("OnDestroy", script.variant, {});
			}
		}
	}

	void ScriptingSystem::ReloadScripts(entt::registry& registry, const std::unordered_map<uint32_t, Ref<Actor>>& actorPool)
	{
		const auto& view = registry.view<BehaviourComponent>();
		view.each([&](BehaviourComponent& behaviour)
		{
			auto& result = actorPool.find(behaviour.ID);
			if (result != actorPool.end())
			{
				behaviour.Actor = result->second;
				behaviour.ID = result->second->GetID();
				behaviour.Scripts.clear();

				for (auto& [name, data] : behaviour.OutValues)
				{
					ScriptInstance instance = {};
					{
						instance.type = rttr::type::get_by_name(name);
						instance.variant = instance.type.create();
					}

					auto& primitive = instance.variant.get_wrapped_value_non_const<BehaviourPrimitive>();
					primitive.m_Actor = behaviour.Actor;

					for (const auto& value : data.OutValues)
					{
						switch (value.Value.index())
						{

						case (uint32_t)OutValueType::Float:
						{
							for (const auto& [varName, varValue] : primitive.m_OutFloatVariables)
							{
								if (value.Key == varName)
								{
									*varValue = std::get<float>(value.Value);
									break;
								}
							}

							break;
						}
						case (uint32_t)OutValueType::Int:
						{
							for (const auto& [varName, varValue] : primitive.m_OutIntVariables)
							{
								if (value.Key == varName)
								{
									*varValue = std::get<int>(value.Value);
									break;
								}
							}

							break;
						}
						case (uint32_t)OutValueType::String:
						{
							for (const auto& [varName, varValue] : primitive.m_OutStringVariables)
							{
								if (value.Key == varName)
								{
									*varValue = std::get<std::string>(value.Value);
									break;
								}
							}

							break;
						}
						default:

							break;
						}
					}

					behaviour.Scripts.push_back(instance);
				}

			}
			else
			{
				NATIVE_ERROR("ReloadScripts:: Actor <{}> not found!", behaviour.ID);
			}
		});
	}

	void ScriptingSystem::OnCollisionBegin(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		auto& admin = WorldAdmin::GetSingleton();
		if (admin->GetActiveScene().HasComponent<BehaviourComponent>(*actorB))
		{
			BehaviourComponent* behaviour = admin->GetActiveScene().GetComponent<BehaviourComponent>(*actorB);

			for (auto& instance : behaviour->Scripts)
			{
				auto& primitive = instance.variant.get_wrapped_value_non_const<BehaviourPrimitive>();
				primitive.OnCollisionContact(actorA, isTrigger);
			}
		}
	}

	void ScriptingSystem::OnCollisionEnd(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		auto& admin = WorldAdmin::GetSingleton();
		if (admin->GetActiveScene().HasComponent<BehaviourComponent>(*actorB))
		{
			BehaviourComponent* behaviour = admin->GetActiveScene().GetComponent<BehaviourComponent>(*actorB);

			for (auto& instance : behaviour->Scripts)
			{
				auto& primitive = instance.variant.get_wrapped_value_non_const<BehaviourPrimitive>();
				primitive.OnCollisionExit(actorA, isTrigger);
			}
		}
	}

	void ScriptingSystem::OnDebugDraw(entt::registry& registry)
	{
		const auto& view = registry.view<BehaviourComponent>();
		view.each([&](BehaviourComponent& behaviour)
		{
			for (auto& instance : behaviour.Scripts)
			{
				auto& primitive = instance.variant.get_wrapped_value_non_const<BehaviourPrimitive>();
				primitive.OnDebugDraw();
			}
		});
	}
}