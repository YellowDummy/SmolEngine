project "Yojimbo"
    kind "StaticLib"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    platforms { "x64" }

    yojimbo_version = "1.0"
    libs = { "sodium", "mbedtls", "mbedx509", "mbedcrypto" }

    if os.istarget "windows" then
        includedirs { ".", "./windows", "netcode.io", "reliable.io" }
        libdirs { "./windows" }
    else
        includedirs { ".", "/usr/local/include", "netcode.io", "reliable.io" }
    end

    filter "system:windows"
    systemversion "latest"
    staticruntime "On"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    links { libs }
    warnings "Extra"
    floatingpoint "Fast"
    vectorextensions "SSE2"

    defines { "NETCODE_ENABLE_TESTS=1", "RELIABLE_ENABLE_TESTS=1" }
    files { "yojimbo.h", "yojimbo.cpp", "tlsf/tlsf.h", "tlsf/tlsf.c", "netcode.io/netcode.c", "netcode.io/netcode.h", "reliable.io/reliable.c", "reliable.io/reliable.h" }

    defines 
    { 
        "_CRT_SECURE_NO_WARNINGS"
    }

    --------------------------------------- Debug

	filter "configurations:Debug (Vulkan)"
    buildoptions "/MDd"
    symbols "on"

	filter "configurations:Debug (OpenGL)"
    buildoptions "/MDd"
    symbols "on"

	--------------------------------------- Release

	filter "configurations:Release (Vulkan)"
    defines { "NDEBUG" }
    buildoptions "/MD"
    optimize "on"

	filter "configurations:Release (OpenGL)"
    defines { "NDEBUG" }
    buildoptions "/MD"
    optimize "on"
