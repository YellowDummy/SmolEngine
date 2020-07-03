workspace "SmolEngine"
architecture "x64"
configuration
{
"Debug",
"Release",
"Dist"
}

outDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}";

project "SmolEngine"
location "SmolEngine"
kind "SharedLib"
language "C++"
tagetdir ("bin/" .. outDir.. "/%{prj.name}")
objdir ("bin-int/" .. outDir.. "/%{prj.name}")

files
{
"%{prj.name}/scr/**.h",
"%{prj.name}/scr/**.cpp"
}

includedirs
{
"%{prj.name}/libraries/spdlog/include"
}

filter "system:widnows"
{

cppdialect "C++17"
staticruntime "On"
systemversion "latest"

defines
{
"PLATFORM_WIN",
"BUILD_DLL"
}

postbuildcommands
{
("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outDir .. "/GameX")
}

}


filter "configuration:Debug"
defines "DEBUG"
symbols "On"

filter "configuration:Release"
defines "RELEASE"
optimize "On"

filter "configuration:Dist"
defines "DIST"
optimize "On"

//----------------------------------------------------------------
project "GameX"
location "GameX"
kind "ConsoleApp"
language "C++"

tagetdir ("bin/" .. outDir.. "/%{prj.name}")
objdir ("bin-int/" .. outDir.. "/%{prj.name}")

files
{
"%{prj.name}/scr/**.h",
"%{prj.name}/scr/**.cpp"
}

includedirs
{
"%{prj.name}/libraries/spdlog/include",
"SmolEngine/scr"

}

links
{
"SmolEngine"
}

filter "system:widnows"
{
cppdialect "C++17"
staticruntime "On"
systemversion "latest"

defines
{
"PLATFORM_WIN",
}

filter "configuration:Debug"
defines "DEBUG"
symbols "On"

filter "configuration:Release"
defines "RELEASE"
optimize "On"

filter "configuration:Dist"
defines "DIST"
optimize "On"