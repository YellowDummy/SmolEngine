project "Yaml-cpp"
	kind "StaticLib"
	language "C++"
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"../../smolengine/include/Libraries/yaml-cpp/include/"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	symbols "on"

	filter "configurations:Release"
	buildoptions "/MD"
	buildoptions "/bigobj"
	optimize "on"


