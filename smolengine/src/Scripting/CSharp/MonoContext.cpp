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
	void PrintMethod(MonoString* string)
	{
		char* cppString = mono_string_to_utf8(string);

		std::cout << cppString;

		mono_free(cppString);
	}

	MonoContext::MonoContext()
	{
		//Indicate Mono where you installed the lib and etc folders
		mono_set_dirs("../vendor/mono/lib", "../vendor/mono/etc");

		//Create the main CSharp domain
		MonoDomain* domain = mono_jit_init("CSharp_Domain");

		//Load the binary file as an Assembly
		MonoAssembly* csharpAssembly = mono_domain_assembly_open(domain, "../vendor/mono/CSharp/Debug/CSharp.exe");
		if (!csharpAssembly)
		{
			//Error detected
			RUNTIME_ERROR("Failed to create mono context");
		}

		s_Instance = this;
		m_Domain = domain;

		/* we usually get the class we need during initialization */
		MonoImage* image = mono_assembly_get_image(csharpAssembly);
		MonoClass* my_class = mono_class_from_name(image, "SmolEngine", "Actor");

		{
			//SetUp Internal Calls called from CSharp
			//Namespace.Class::Method + a Function pointer with the actual definition
			mono_add_internal_call("SmolEngine.CppAPI::GetSetTransformComponent", &GetSetTransformComponentCSharp);
			mono_add_internal_call("SmolEngine.CppAPI::GetSetHeadComponent", &GetSetHeadComponentCSharp);

			int argc = 1;
			char* argv[1] = { (char*)"CSharp" };

			//Call the main method in this code
			mono_jit_exec(domain, csharpAssembly, argc, argv);
		}

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
	}

	MonoDomain* MonoContext::GetDomain()
	{
		return s_Instance->m_Domain;
	}
}