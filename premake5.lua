
workspace "SmolEngine"
	architecture "x64"
	startproject "SmolEngine-Editor"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
include "vendor/box2d"
include "vendor/yaml-cpp"
group ""


project "SmolEngine"
	location "smolengine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("vendor/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
	pchsource "smolengine/src/stdafx.cpp"

	files
	{
		"smolengine/include/**.h",
		"smolengine/src/**.cpp"
	}

	includedirs
	{
		"smolengine/include/",
		"smolengine/include/Libraries/",
		"smolengine/include/Libraries/yaml-cpp/include/",
		"smolengine/include/Libraries/box_2D/include/",
		"smolengine/include/Libraries/fmod/include/",
		"smolengine/include/Libraries/Frostium3D/",
		"smolengine/include/Libraries/Frostium3D/Libraries/",
		"smolengine/include/Libraries/Frostium3D/Libraries/vulkan/",
		"smolengine/include/Libraries/Frostium3D/Libraries/spdlog/include/",
		"smolengine/include/Libraries/Frostium3D/Libraries/cereal/include/",
		"smolengine/include/Libraries/Frostium3D/Libraries/taskflow/",
		"smolengine/include/Libraries/Frostium3D/Libraries/glm/",
		"vendor/bullet3/include/"
	}

	links 
	{ 
		"Box2D",
		"Yaml-cpp",
		"vendor/fmod/libs/fmodL_vc.lib",
		"vendor/fmod/libs/fmodstudioL_vc.lib"
	}

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"FROSTIUM_SMOLENGINE_IMPL"
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PLATFORM_WIN",
			"BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		buildoptions "/Zm500"
		symbols "on"

		links 
		{ 
		   "vendor/frostium/libs/Frostium_d.lib",
		   "vendor/bullet3/libs/BulletCollision_Debug.lib",
		   "vendor/bullet3/libs/BulletDynamics_Debug.lib",
		   "vendor/bullet3/libs/LinearMath_Debug.lib"
		}

		defines
		{
			"SMOLENGINE_DEBUG",
			"_DEBUG"
		}
		

	filter "configurations:Release"
		buildoptions "/MD"
		buildoptions "/bigobj"
		buildoptions "/Zm500"
		optimize "on"

		links 
		{ 
		   "vendor/frostium/libs/Frostium.lib",
		   "vendor/bullet3/libs/BulletCollision.lib",
		   "vendor/bullet3/libs/BulletDynamics.lib",
		   "vendor/bullet3/libs/LinearMath.lib"
		}

		defines
		{
			"NDEBUG"
		}


group "Samples"
project "SponzaShooter"
	location "samples"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("vendor/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"samples/Scripts.h",
		"samples/Scripts.cpp",
		"samples/SponzaShooter.h",
		"samples/SponzaShooter.cpp",
	}

	includedirs
	{
		"smolengine-editor/include/",
		"smolengine-editor/src/",
		"smolengine/include/",
		"smolengine/include/Libraries/",
		"smolengine/include/Libraries/box_2D/include/",
		"smolengine/include/Libraries/fmod/include/",
		"smolengine/include/Libraries/Frostium3D/",
		"smolengine/include/Libraries/Frostium3D/Libraries/",
		"smolengine/include/Libraries/Frostium3D/Libraries/taskflow/",
		"smolengine/include/Libraries/Frostium3D/Libraries/vulkan/",
		"smolengine/include/Libraries/Frostium3D/Libraries/spdlog/include/",
		"smolengine/include/Libraries/Frostium3D/Libraries/cereal/include/",
		"smolengine/include/Libraries/Frostium3D/Libraries/glm/",
	}

	links
	{
		"SmolEngine",
		"vendor/fmod/libs/fmodL_vc.lib",
		"vendor/fmod/libs/fmodstudioL_vc.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"PLATFORM_WIN"
		}

	filter "configurations:Debug"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	symbols "on"

	filter "configurations:Release"
	buildoptions "/MD"
	buildoptions "/bigobj"
	optimize "on"
	group ""
	
	project "SmolEngine-Editor"

	location "smolengine-editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("vendor/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"smolengine-editor/include/**.h",
		"smolengine-editor/src/**.cpp",
	}

	includedirs
	{
		"smolengine-editor/include/",
		"smolengine-editor/src/",
		"smolengine/include/",
		"smolengine/include/Libraries/",
		"smolengine/include/Libraries/box_2D/include/",
		"smolengine/include/Libraries/fmod/include/",
		"smolengine/include/Libraries/Frostium3D/",
		"smolengine/include/Libraries/Frostium3D/Libraries/",
		"smolengine/include/Libraries/Frostium3D/Libraries/taskflow/",
		"smolengine/include/Libraries/Frostium3D/Libraries/vulkan/",
		"smolengine/include/Libraries/Frostium3D/Libraries/spdlog/include/",
		"smolengine/include/Libraries/Frostium3D/Libraries/cereal/include/",
		"smolengine/include/Libraries/Frostium3D/Libraries/glm/",
	}

	links
	{
		"SmolEngine",
		"vendor/fmod/libs/fmodL_vc.lib",
		"vendor/fmod/libs/fmodstudioL_vc.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"PLATFORM_WIN"
		}

	filter "configurations:Debug"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	symbols "on"

	filter "configurations:Release"
	buildoptions "/MD"
	buildoptions "/bigobj"
	optimize "on"