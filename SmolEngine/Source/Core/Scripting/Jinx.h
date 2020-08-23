#pragma once
#include "Core/Core.h"
#include "Core/SLog.h"
#include "Core/ECS/ScriptableObject.h"

#include <string>
#include <fstream>
#include <Jinx.hpp>

namespace SmolEngine
{
	class Actor;

	template<typename T>
	Jinx::Variant CallFunc(Jinx::ScriptPtr script, Jinx::Parameters params)
	{
		auto someObject = std::any_cast<T*>(script->GetUserContext());
		someObject->Start();
		return nullptr;
	}

	template<typename T>
	void RegisterScriptFunctions(const Jinx::RuntimePtr runtime)
	{
		auto library = runtime->GetLibrary("smolengine");
		library->RegisterFunction(
			Jinx::Visibility::Public,
			"call start func",
			CallFunc<T>);
	}

	class JinxScript: public ScriptableObject
	{
	public:
		JinxScript(Ref<Actor> actor, Jinx::RuntimePtr runtime, std::string& filePath = std::string(""))
			:ScriptableObject(actor), m_Runtime(runtime) 
		{
			RegisterScriptFunctions<JinxScript>(m_Runtime);

			std::ifstream ifs(filePath);
			std::string content((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));

			m_Script = m_Runtime->CreateScript(content.c_str(), this);
			if (m_Script)
			{
				m_Script->Execute();
			}
			else
			{
				EDITOR_ERROR("JinxScript initialization failed!");
			}


		}

		void Start() override
		{

		}

		void OnUpdate(DeltaTime deltaTime) override
		{

		}

		void OnDestroy() override
		{

		}


	private:
		Jinx::RuntimePtr m_Runtime;
		Jinx::ScriptPtr m_Script = nullptr;
	};
}