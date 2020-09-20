

project "Yojimbo"
    kind "StaticLib"
    language "C++"
    platforms { "x64" }

    yojimbo_version = "1.0"
    libs = { "sodium", "mbedtls", "mbedx509", "mbedcrypto" }

    if os.istarget "windows" then
        includedirs { ".", "./windows", "netcode.io", "reliable.io" }
        libdirs { "./windows" }
    else
        includedirs { ".", "/usr/local/include", "netcode.io", "reliable.io" }
        targetdir "bin/"  
    end

    filter "system:windows"
    systemversion "latest"
    staticruntime "On"
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

    filter "configurations:Debug"
    defines "SE_DEBUG"
    buildoptions "/MDd"
    buildoptions "/bigobj"
    symbols "on"

filter "configurations:Release"
    defines "SE_RELEASE"
    defines { "NDEBUG" }
    buildoptions "/MD"
    buildoptions "/bigobj"
    optimize "on"

filter "configurations:Dist"
    defines "SE_DIST"
    defines { "NDEBUG" }
    buildoptions "/MD"
    buildoptions "/bigobj"
    optimize "on"
