workspace "SmolEngine"
	architecture "x64"
	startproject "GameX"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "SmolEngine/Libraries/glfw/include"
IncludeDir["Glad"] = "SmolEngine/Libraries/glad/include"
IncludeDir["ImGui"] = "Hazel/vendor/imgui"

group "Dependencies"
include "SmolEngine/Libraries/glfw"
include "SmolEngine/Libraries/glad"
include "SmolEngine/Libraries/imgui"

group ""


project "SmolEngine"
	location "SmolEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
	pchsource "SmolEngine/Source/Core/stdafx.cpp"

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Source",
		"%{prj.name}/Libraries/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PLATFORM_WIN",
			"BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/GameX")
		}

	filter "configurations:Debug"
		defines "SE_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "SE_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "SE_DIST"
		buildoptions "/MD"
		optimize "On"

project "GameX"
	location "GameX"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"SmolEngine/Libraries/spdlog/include",
		"SmolEngine/Source",
		"SmolEngine/Libraries"

	}

	links
	{
		"SmolEngine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"PLATFORM_WIN"
		}

	filter "configurations:Debug"
		defines "SE_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "SE_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "SE_DIST"
		buildoptions "/MD"
		optimize "On"