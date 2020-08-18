workspace "SmolEngine"
	architecture "x64"
	startproject "SmolEngine-Editor"

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
IncludeDir["ImGui"] = "SmolEngine/Libraries/imgui"
IncludeDir["glm"] = "SmolEngine/Libraries/glm"
IncludeDir["stb"] = "SmolEngine/Libraries/stb_image"
IncludeDir["entt"] = "SmolEngine/Libraries/entt"

group "Dependencies"
include "SmolEngine/Libraries/glfw"
include "SmolEngine/Libraries/glad"
include "SmolEngine/Libraries/imgui"

group ""


project "SmolEngine"
	location "SmolEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
	pchsource "SmolEngine/Source/stdafx.cpp"

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/Libraries/glm/glm/**.hpp",
		"%{prj.name}/Libraries/glm/glm/**.inl",
		"%{prj.name}/Libraries/stb_image/**.h",
		"%{prj.name}/Libraries/stb_image/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Source",
		"%{prj.name}/Libraries/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
           "%{IncludeDir.entt}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
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
		defines "SE_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "SE_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "SE_DIST"
		buildoptions "/MD"
		optimize "on"

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
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp",
		"%{prj.name}/Libraries/glm/glm/**.hpp",
		"%{prj.name}/Libraries/glm/glm/**.inl",
	}

	includedirs
	{
		"SmolEngine/Libraries/spdlog/include",
		"SmolEngine/Source",
		"SmolEngine/Libraries",
		"%{IncludeDir.glm}",
           "%{IncludeDir.entt}"
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
		symbols "on"

	filter "configurations:Release"
		defines "SE_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "SE_DIST"
		buildoptions "/MD"
		optimize "on"



	project "SmolEngine-Editor"
	location "SmolEngine-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp",
		"%{prj.name}/Libraries/glm/glm/**.hpp",
		"%{prj.name}/Libraries/glm/glm/**.inl",
	}

	includedirs
	{
		"SmolEngine/Libraries/spdlog/include",
		"SmolEngine/Source",
		"SmolEngine/Libraries",
		"%{IncludeDir.glm}",
           "%{IncludeDir.entt}"
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
		symbols "on"

	filter "configurations:Release"
		defines "SE_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "SE_DIST"
		buildoptions "/MD"
		optimize "on"
