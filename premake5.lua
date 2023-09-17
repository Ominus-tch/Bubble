workspace "Bubble"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directiories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Bubble/vendor/GLFW/include"

include "Bubble/vendor/GLFW"

project "Bubble"
    location "Bubble"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    --pchheader "bgpch.h"
    --pchsource "Bubble/src/bgpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
    }

    links 
    {
        "GLFW",
        "opengl32.lib"
    }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines 
        {
            "BG_PLATFORM_WINDOWS",
            "BG_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines "BG_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "BG_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "BG_DIST"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Bubble/vendor/spdlog/include",
        "Bubble/src"
    }

    links
    {
        "Bubble"
    }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines 
        {
            "BG_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "BG_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "BG_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "BG_DIST"
        optimize "On"