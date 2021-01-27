workspace "SmolEngine"
	architecture "x64"
	startproject "SmolEngine-Editor"

	configurations
	{
		"Debug (Vulkan)",
		"Release (Vulkan)",
		"Debug (OpenGL)",
		"Release (OpenGL)",
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
IncludeDir["rttr"] = "SmolEngine/Libraries/rttr/src"
IncludeDir["vulkan"] = "SmolEngine/Libraries/vulkan/include"
IncludeDir["imgizmo"] = "SmolEngine/Libraries/imgizmo/src"
IncludeDir["ktx"] = "SmolEngine/Libraries/ktx/include"

group "Dependencies"
include "SmolEngine/Libraries/glfw"
include "SmolEngine/Libraries/glad"
include "SmolEngine/Libraries/imgui"
include "SmolEngine/Libraries/yojimbo"
include "SmolEngine/Libraries/box2d"
include "SmolEngine/Libraries/rttr"
include "SmolEngine/Libraries/spir_v_cross"
include "SmolEngine/Libraries/ktx"
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

		"%{prj.name}/Libraries/imgizmo/src/ImGuizmo.h",
		"%{prj.name}/Libraries/imgizmo/src/ImGuizmo.cpp",
		"%{prj.name}/Libraries/imgizmo/src/ImCurveEdit.h",
		"%{prj.name}/Libraries/imgizmo/src/ImGradient.h",
		"%{prj.name}/Libraries/imgizmo/src/ImGradient.cpp",
		"%{prj.name}/Libraries/imgizmo/src/ImCurveEdit.cpp",
		"%{prj.name}/Libraries/imgizmo/src/ImSequencer.h",
		"%{prj.name}/Libraries/imgizmo/src/ImSequencer.cpp",

	}

	includedirs
	{
		"SmolEngine/Libraries/icon_font_cpp_headers",
		"SmolEngine/Libraries/spdlog/include",
		"SmolEngine/Libraries/cereal/include",
		"SmolEngine/Libraries/freetype/include/",
		"SmolEngine/Libraries/fmod/include",
		"SmolEngine/Libraries/box2d/include",
		"SmolEngine/Libraries/assimp/include",
		
		"%{prj.name}/Source",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
        "%{IncludeDir.entt}",
		"%{IncludeDir.yojimbo}",
		"%{IncludeDir.rttr}",
		"%{IncludeDir.vulkan}",
		"%{IncludeDir.imgizmo}",
		"%{IncludeDir.ktx}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"Yojimbo",
		"Box2D",
		"RTTR",
		"SPIRV-Cross",
		"KTX-Tools",
		
		"SmolEngine/Libraries/vulkan/libs/vulkan-1.lib",
		"SmolEngine/Libraries/vulkan/libs/VkLayer_utils.lib",
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

	filter "configurations:Debug (Vulkan)"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		buildoptions "/Zm500"
		symbols "on"

		links 
		{ 
			"SmolEngine/Libraries/freetype/libs/freetype_d.lib",
			"SmolEngine/Libraries/vulkan/libs/shaderc_d.lib",
			"SmolEngine/Libraries/vulkan/libs/shaderc_util_d.lib",
			"SmolEngine/Libraries/vulkan/libs/glslang_d.lib",
			"SmolEngine/Libraries/vulkan/libs/SPIRV_d.lib",
			"SmolEngine/Libraries/vulkan/libs/SPIRV-Tools_d.lib",
			"SmolEngine/Libraries/vulkan/libs/SPIRV-Tools-opt_d.lib",
			"SmolEngine/Libraries/vulkan/libs/machineIndependent_d.lib",
			"SmolEngine/Libraries/vulkan/libs/genericCodeGen_d.lib",
			"SmolEngine/Libraries/vulkan/libs/OGLCompiler_d.lib",
			"SmolEngine/Libraries/vulkan/libs/OSDependent_d.lib",

			"SmolEngine/Libraries/assimp/libs/assimp-vc142-mtd.lib",
			"SmolEngine/Libraries/assimp/libs/zlibstaticd.lib"
		}

		defines
		{
			"SMOLENGINE_DEBUG"
		}

	filter "configurations:Debug (OpenGL)"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	buildoptions "/Zm500"
	symbols "on"

	links 
	{ 
		"opengl32.lib",
		"SmolEngine/Libraries/freetype/libs/freetype_d.lib",
		"SmolEngine/Libraries/vulkan/libs/shaderc_d.lib",
		"SmolEngine/Libraries/vulkan/libs/shaderc_util_d.lib",
		"SmolEngine/Libraries/vulkan/libs/glslang_d.lib",
		"SmolEngine/Libraries/vulkan/libs/SPIRV_d.lib",
		"SmolEngine/Libraries/vulkan/libs/SPIRV-Tools_d.lib",
		"SmolEngine/Libraries/vulkan/libs/SPIRV-Tools-opt_d.lib",
		"SmolEngine/Libraries/vulkan/libs/machineIndependent_d.lib",
		"SmolEngine/Libraries/vulkan/libs/genericCodeGen_d.lib",
		"SmolEngine/Libraries/vulkan/libs/OGLCompiler_d.lib",
		"SmolEngine/Libraries/vulkan/libs/OSDependent_d.lib",

		"SmolEngine/Libraries/assimp/libs/assimp-vc142-mtd.lib",
		"SmolEngine/Libraries/assimp/libs/zlibstaticd.lib"
	}

	defines
	{
		"SMOLENGINE_OPENGL_IMPL",
		"SMOLENGINE_DEBUG"
	}

	filter "configurations:Release (Vulkan)"
		buildoptions "/MD"
		buildoptions "/bigobj"
		buildoptions "/Zm500"
		optimize "on"

		links 
		{ 
			"SmolEngine/Libraries/freetype/libs/freetype.lib",
			"SmolEngine/Libraries/vulkan/libs/shaderc.lib",
			"SmolEngine/Libraries/vulkan/libs/shaderc_util.lib",
			"SmolEngine/Libraries/vulkan/libs/glslang.lib",
			"SmolEngine/Libraries/vulkan/libs/SPIRV.lib",
			"SmolEngine/Libraries/vulkan/libs/SPIRV-Tools.lib",
			"SmolEngine/Libraries/vulkan/libs/SPIRV-Tools-opt.lib",
			"SmolEngine/Libraries/vulkan/libs/machineIndependent.lib",
			"SmolEngine/Libraries/vulkan/libs/genericCodeGen.lib",
			"SmolEngine/Libraries/vulkan/libs/OGLCompiler.lib",
			"SmolEngine/Libraries/vulkan/libs/OSDependent.lib",

			"SmolEngine/Libraries/assimp/libs/assimp-vc142-mt.lib",
			"SmolEngine/Libraries/assimp/libs/zlibstatic.lib"
		}

	filter "configurations:Release (OpenGL)"
	buildoptions "/MD"
	buildoptions "/bigobj"
	buildoptions "/Zm500"
	optimize "on"

	   links 
	   { 
		"opengl32.lib",
		 "SmolEngine/Libraries/freetype/libs/freetype.lib",
		 "SmolEngine/Libraries/vulkan/libs/shaderc.lib",
		 "SmolEngine/Libraries/vulkan/libs/shaderc_util.lib",
		 "SmolEngine/Libraries/vulkan/libs/glslang.lib",
		 "SmolEngine/Libraries/vulkan/libs/SPIRV.lib",
		 "SmolEngine/Libraries/vulkan/libs/SPIRV-Tools.lib",
		 "SmolEngine/Libraries/vulkan/libs/SPIRV-Tools-opt.lib",
		 "SmolEngine/Libraries/vulkan/libs/machineIndependent.lib",
		 "SmolEngine/Libraries/vulkan/libs/genericCodeGen.lib",
		 "SmolEngine/Libraries/vulkan/libs/OGLCompiler.lib",
		 "SmolEngine/Libraries/vulkan/libs/OSDependent.lib",

		 "SmolEngine/Libraries/assimp/libs/assimp-vc142-mt.lib",
		 "SmolEngine/Libraries/assimp/libs/zlibstatic.lib"
	   }

	    defines
	   {
		 "SMOLENGINE_OPENGL_IMPL"
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
		"SmolEngine/Libraries",

		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.yojimbo}",
		"%{IncludeDir.rttr}",
		"%{IncludeDir.vulkan}"
	}

	links
	{
		"SmolEngine",
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


	--------------------------------------- Debug

	filter "configurations:Debug (Vulkan)"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	symbols "on"

	filter "configurations:Debug (OpenGL)"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	symbols "on"

	defines
	{
		"SMOLENGINE_OPENGL_IMPL"
	}

	--------------------------------------- Release

	filter "configurations:Release (Vulkan)"
	buildoptions "/MD"
	buildoptions "/bigobj"
	optimize "on"

	filter "configurations:Release (OpenGL)"
	buildoptions "/MD"
	buildoptions "/bigobj"
	optimize "on"

	defines
	{
		"SMOLENGINE_OPENGL_IMPL"
	}

	
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
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.yojimbo}",
		"%{IncludeDir.rttr}",
		"%{IncludeDir.vulkan}",
		"%{IncludeDir.imgizmo}"
	}

	links
	{
		"SmolEngine",
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

	--------------------------------------- Debug

	filter "configurations:Debug (Vulkan)"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	symbols "on"

	filter "configurations:Debug (OpenGL)"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	symbols "on"

	defines
	{
		"SMOLENGINE_OPENGL_IMPL"
	}

	--------------------------------------- Release

	filter "configurations:Release (Vulkan)"
	buildoptions "/MD"
	buildoptions "/bigobj"
	optimize "on"

	filter "configurations:Release (OpenGL)"
	buildoptions "/MD"
	buildoptions "/bigobj"
	optimize "on"

	defines
	{
		"SMOLENGINE_OPENGL_IMPL"
	}