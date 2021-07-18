#include "stdafx.h"
#include "ECS/Components/Singletons/ScriptingSystemStateSComponent.h"

#include "Scripting/CPP/MetaContext.h"
#include "Scripting/CSharp/MonoContext.h"

namespace SmolEngine
{
	ScriptingSystemStateSComponent::ScriptingSystemStateSComponent()
	{
		m_MetaContext = new MetaContext();
		m_MonoContext = new MonoContext();
		Instance = this;
	}

	ScriptingSystemStateSComponent::~ScriptingSystemStateSComponent()
	{
		delete m_MetaContext, m_MonoContext;
		Instance = nullptr;
	}

	ScriptingSystemStateSComponent* ScriptingSystemStateSComponent::GetSingleton()
	{
		return Instance;
	}
}