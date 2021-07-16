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

	void Log(const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void* user_data)
	{
		NATIVE_INFO(message);
	}

	void Print(const char* string, mono_bool is_stdout)
	{
		NATIVE_INFO(string);
	}

	MonoContext::MonoContext()
	{
		mono_set_dirs("../vendor/mono/lib", "../vendor/mono/etc");
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		mono_config_parse(NULL);
		mono_set_signal_chaining(true);
		mono_trace_set_log_handler(Log, nullptr);
		mono_trace_set_print_handler(Print);
		mono_trace_set_printerr_handler(Print);

		s_Instance = this;
		m_RootDomain = mono_jit_init("CSharp_Domain");

		LoadDomain();
		LoadMonoImage();
		LoadAssembly();
	}

	void MonoContext::Shutdown()
	{
		if (m_CSharpAssembly != nullptr && m_Domain != m_RootDomain)
		{
			mono_domain_set(m_RootDomain, false);
			mono_image_close(m_Image);
			mono_domain_finalize(m_Domain, 2000);
			mono_domain_unload(m_Domain);

			m_DefaultClasses.clear();
			m_ClassNames.clear();
			m_Domain = nullptr;
			m_CSharpAssembly = nullptr;
			m_Image = nullptr;
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

	void MonoContext::Track()
	{
		std::filesystem::path p("../vendor/mono/CSharp/Debug/CSharp.exe");
		if (std::filesystem::exists(p))
		{
			auto time = std::filesystem::last_write_time(p);
			if (time != m_LastWriteTime)
			{
				NATIVE_INFO("C# module: Reloading...");
				OnRecompilation();
				NATIVE_INFO("C# module: Reloading complete!");
			}
		}
	}

	void MonoContext::OnRecompilation()
	{
		Shutdown();
		LoadDomain();
		LoadMonoImage();
		LoadAssembly();
	}

	void MonoContext::LoadMonoImage()
	{
		MonoImageOpenStatus status;
		std::ifstream instream(m_DLLPath.c_str(), std::ios::in | std::ios::binary);
		std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());

		m_Image = mono_image_open_from_data_with_name(
			(char*)&data[0], data.size(),
			true, &status, false,
			"SmolEngine");

		if (status != MONO_IMAGE_OK)
		{
			RUNTIME_ERROR("Failed to create mono context");
		}

		// denug symbols
		std::filesystem::path p(m_DLLPath);
		std::string pdbPath = p.parent_path().u8string() + "/" + p.filename().stem().u8string() + ".pdb";

		instream = std::ifstream(pdbPath.c_str(), std::ios::in | std::ios::binary);
		data = std::vector<uint8_t>((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
		//mono_debug_open_image_from_memory(m_Image, &data[0], data.size());

	}

	void MonoContext::LoadDomain()
	{
		std::string name = "SmolEngine.ScriptsDomain";
		m_Domain = mono_domain_create_appdomain((char*)name.c_str(), NULL);
		mono_domain_set(m_Domain, false);
	}

	void MonoContext::LoadAssembly(bool is_initialization)
	{
		MonoImageOpenStatus status;
		m_CSharpAssembly = mono_assembly_load_from_full(m_Image, "SmolEngine", &status, false);

		if (m_Domain && mono_image_get_entry_point(m_Image))
		{
			ResolveFunctions();
			ResolveClasses();
			GetClassNames();

			m_LastWriteTime = std::filesystem::last_write_time(m_DLLPath);
		}
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
		m_DefaultClasses[ClassDefs::Actor] = mono_class_from_name(m_Image, "SmolEngine", "Actor");
		m_DefaultClasses[ClassDefs::Reflection] = mono_class_from_name(m_Image, "SmolEngine", "Reflection");
	}

	void MonoContext::GetClassNames()
	{
		{
			MonoObject* instance = mono_object_new(m_Domain, m_DefaultClasses[ClassDefs::Reflection]);
			mono_runtime_object_init(instance);
		}

		{
			MonoObject* instance = mono_object_new(m_Domain, m_DefaultClasses[ClassDefs::Actor]);

			/* allocate memory for the object */
			MonoMethodDesc* desc = mono_method_desc_new(":.ctor(uint)", FALSE);
			MonoMethod* ctor = mono_method_desc_search_in_class(desc, m_DefaultClasses[ClassDefs::Actor]);
			mono_method_desc_free(desc);

			void* args[1];
			uint32_t id = 266;
			args[0] = &id;

			auto p = mono_runtime_invoke(ctor, instance, args, NULL);

		}
	}
}