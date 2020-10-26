project "RTTR"
	kind "StaticLib"
	language "C++"
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src/",
		"src/rttr"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS"
		}


		filter "configurations:Debug"
		defines "SE_DEBUG"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		symbols "on"

	filter "configurations:Release"
		defines "SE_RELEASE"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"

	filter "configurations:Dist"
		defines "SE_DIST"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"
