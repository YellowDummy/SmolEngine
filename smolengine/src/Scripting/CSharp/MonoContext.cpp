#include "stdafx.h"
#include "Scripting/CSharp/MonoContext.h"
#include "Scripting/CSharp/CSharpAPI.h"

#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>
#include <mono/utils/mono-counters.h>
#include <mono/utils/mono-logger.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/profiler.h>
#include <mono/metadata/debug-helpers.h>


namespace SmolEngine
{
	void AddClass(MonoString* mono_str)
	{
		char* cpp_str = mono_string_to_utf8(mono_str);
		MonoContext::GetSingleton()->m_ClassNames.emplace_back(cpp_str);
		mono_free(cpp_str);
	}

	MonoContext::MonoContext()
	{
		mono_set_dirs("../vendor/mono/lib", "../vendor/mono/etc");

		m_Domain = mono_jit_init("CSharp_Domain");
		s_Instance = this;

		CreateAssembly();
		ResolveFunctions();
		ResolveClasses();
		Run();
		GetClassNames();

		OnRecompilation();

#if 0
		{
			/* allocate memory for the object */
			MonoMethodDesc* desc = mono_method_desc_new(":.ctor(uint)", FALSE);
			MonoMethod* ctor = mono_method_desc_search_in_class(desc, my_class);
			mono_method_desc_free(desc);

			void* args[1];
			uint32_t id = 266;
			args[0] = &id;

			MonoObject* my_class_instance = mono_object_new(domain, my_class);
			auto p = mono_runtime_invoke(ctor, my_class_instance, args, NULL);
	}
#endif 
	}

	void MonoContext::Run()
	{
		// Call the main method in this code
		int argc = 1;
		char* argv[1] = { (char*)"CSharp" };
		mono_jit_exec(m_Domain, m_CSharpAssembly, argc, argv);
	}

	void MonoContext::Shutdown()
	{
		if (m_CSharpAssembly != nullptr)
		{
			mono_assemblies_cleanup();

			m_DefaultClasses.clear();
			m_ClassNames.clear();
			m_CSharpAssembly = nullptr;
		}
	}

	bool MonoContext::IsRunning()
	{
		return m_CSharpAssembly != nullptr;
	}

	MonoContext* MonoContext::GetSingleton()
	{
		return s_Instance;
	}

	void MonoContext::OnRecompilation()
	{
		Shutdown();
		CreateAssembly();
		ResolveClasses();
		Run();
		GetClassNames();
	}

	void MonoContext::CreateAssembly()
	{
		//Load the binary file as an Assembly
		MonoAssembly* csharpAssembly = mono_domain_assembly_open(m_Domain, "../vendor/mono/CSharp/Debug/CSharp.exe");
		if (!csharpAssembly)
		{
			//Error detected
			RUNTIME_ERROR("Failed to create mono context");
		}

		m_CSharpAssembly = csharpAssembly;
	}

	void MonoContext::ResolveFunctions()
	{
		// SetUp Internal Calls called from CSharp
		// Namespace.Class::Method + a Function pointer with the actual definition
		mono_add_internal_call("SmolEngine.CppAPI::GetSetTransformComponent", &GetSetTransformComponentCSharp);
		mono_add_internal_call("SmolEngine.CppAPI::GetSetHeadComponent", &GetSetHeadComponentCSharp);
		mono_add_internal_call("SmolEngine.Reflection::PushClassName", &AddClass);
	}

	void MonoContext::ResolveClasses()
	{
		// Adds default classes
		MonoImage* image = mono_assembly_get_image(m_CSharpAssembly);

		m_DefaultClasses[ClassDefs::Actor] = mono_class_from_name(image, "SmolEngine", "Actor");
		m_DefaultClasses[ClassDefs::Reflection] = mono_class_from_name(image, "SmolEngine", "Reflection");
	}

	void MonoContext::GetClassNames()
	{
		MonoObject* instance = mono_object_new(m_Domain, m_DefaultClasses[ClassDefs::Reflection]);
		mono_runtime_object_init(instance);
	}
}