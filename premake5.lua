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
IncludeDir["yojimbo"] = "SmolEngine/Libraries/yojimbo"
IncludeDir["rttr"] = "SmolEngine/Libraries/rttr/src/"

group "Dependencies"
include "SmolEngine/Libraries/glfw"
include "SmolEngine/Libraries/glad"
include "SmolEngine/Libraries/imgui"
include "SmolEngine/Libraries/yojimbo"
include "SmolEngine/Libraries/box2d"
include "SmolEngine/Libraries/rttr"
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
		"%{prj.name}/Libraries/stb_image/**.cpp",
		"%{prj.name}/Lua/**.cpp",
		"%{prj.name}/Lua/**.h",
		"%{prj.name}/Lua/**.hpp"
	}

	includedirs
	{
		"SmolEngine/Libraries/icon_font_cpp_headers",
		"SmolEngine/Libraries/spdlog/include",
		"SmolEngine/Libraries/cereal/include",
		"SmolEngine/Libraries/freetype/include/",
		"SmolEngine/Libraries/fmod/include",
		"SmolEngine/Libraries/box2d/include",
		"%{prj.name}/Source",
		"%{prj.name}/Lua",
		"%{prj.name}/Lua/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
        "%{IncludeDir.entt}",
		"%{IncludeDir.yojimbo}",
		"%{IncludeDir.rttr}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"Yojimbo",
		"Box2D",
		"RTTR",
		"opengl32.lib",
		"SmolEngine/Libraries/fmod/libs/fmodL_vc.lib",
		"SmolEngine/Libraries/fmod/libs/fmodstudioL_vc.lib"
	}

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"SMOLENGINE_EDITOR"
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
		buildoptions "/bigobj"
		symbols "on"

		links 
		{ 
			"SmolEngine/Libraries/freetype/libs/freetype_d.lib"
		}

	filter "configurations:Release"
		defines "SE_RELEASE"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"

		links 
		{ 
			"SmolEngine/Libraries/freetype/libs/freetype.lib"
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"

		links 
		{ 
			"SmolEngine/Libraries/freetype/libs/freetype.lib"
		}


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
		"SmolEngine/Libraries/icon_font_cpp_headers",
		"SmolEngine/Libraries/box2d/include",
		"SmolEngine/Libraries/spdlog/include",
		"SmolEngine/Libraries/fmod/include",
		"SmolEngine/Libraries/cereal/include",
		"SmolEngine/Libraries/freetype/include/",
		"SmolEngine/Source",
		"SmolEngine/Lua",
		"SmolEngine/Lua/include",
		"SmolEngine/Libraries",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.yojimbo}",
		"%{IncludeDir.rttr}"
	}

	links
	{
		"SmolEngine",
		"SmolEngine/Lua/lua54.lib",
		"SmolEngine/Libraries/fmod/libs/fmodL_vc.lib",
		"SmolEngine/Libraries/fmod/libs/fmodstudioL_vc.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"PLATFORM_WIN"
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
		"SmolEngine/Libraries/icon_font_cpp_headers",
		"SmolEngine/Libraries/box2d/include",
		"SmolEngine/Libraries/spdlog/include",
		"SmolEngine/Libraries/fmod/include",
		"SmolEngine/Libraries/cereal/include",
		"SmolEngine/Libraries/freetype/include/",
		"SmolEngine/Source",
		"SmolEngine/Libraries",
		"SmolEngine/Lua",
		"SmolEngine/Lua/include",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.yojimbo}",
		"%{IncludeDir.rttr}"
	}

	links
	{
		"SmolEngine",
		"SmolEngine/Lua/lua54.lib",
		"SmolEngine/Libraries/fmod/libs/fmodL_vc.lib",
		"SmolEngine/Libraries/fmod/libs/fmodstudioL_vc.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"PLATFORM_WIN"
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