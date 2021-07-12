#include "stdafx.h"
#include "Scripting/MonoContext.h"

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
	
	void MonoContext::InitMono()
	{
		//Indicate Mono where you installed the lib and etc folders
		mono_set_dirs("../vendor/mono/lib", "../vendor/mono/etc");

		//Create the main CSharp domain
		MonoDomain* domain = mono_jit_init("CSharp_Domain");

		//Load the binary file as an Assembly
		MonoAssembly* csharpAssembly = mono_domain_assembly_open(domain, "D:\Engines\SmolEngine\samples\scripts\CSharp.exe");
		if (!csharpAssembly)
		{
			//Error detected
			RUNTIME_ERROR("Failed to create mono context");
		}

		//SetUp Internal Calls called from CSharp

        //Namespace.Class::Method + a Function pointer with the actual definition
		mono_add_internal_call("GameX.Program::PrintMethod", &PrintMethod);

		int argc = 1;
		char* argv[1] = { (char*)"CSharp" };

		//Call the main method in this code
		mono_jit_exec(domain, csharpAssembly, argc, argv);
	}
}