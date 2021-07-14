#include "stdafx.h"
#include "Scripting/CSharp/MonoContext.h"
#include "Scripting/CSharp/CSharpAPI.h"

#include <iostream>
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

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

		/* we usually get the class we need during initialization */
		MonoImage* image = mono_assembly_get_image(csharpAssembly);
		MonoClass* my_class = mono_class_from_name(image, "SmolEngine", "Test");

		//SetUp Internal Calls called from CSharp

		//Namespace.Class::Method + a Function pointer with the actual definition
		mono_add_internal_call("SmolEngine.Game::PrintMethod", &PrintMethod);
		mono_add_internal_call("SmolEngine.Actor::GetTransform", &GetTransformComponentCSharp);
		mono_add_internal_call("SmolEngine.Actor::HeadComponent", &GetHeadComponentCSharp);

		int argc = 1;
		char* argv[1] = { (char*)"CSharp" };

		//Call the main method in this code
		mono_jit_exec(domain, csharpAssembly, argc, argv);

		/* allocate memory for the object */
		MonoObject* my_class_instance = mono_object_new(domain, my_class);
		/* execute the default argument-less constructor */
		mono_runtime_object_init(my_class_instance);
	}
}