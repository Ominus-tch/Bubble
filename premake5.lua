workspace "Bubble"
    architecture "x64"
    startproject "Sandbox"

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
IncludeDir["Glad"] = "Bubble/vendor/Glad/include"
IncludeDir["ImGui"] = "Bubble/vendor/imgui"

include "Bubble/vendor/GLFW"
include "Bubble/vendor/Glad"
include "Bubble/vendor/imgui"

project "Bubble"
    location "Bubble"
    kind "SharedLib"
    language "C++"
    staticruntime "off"

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
        cppdialect "C++20"
        systemversion "latest"

        defines 
        {
            "BG_PLATFORM_WINDOWS",
            "BG_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
        }

    filter "configurations:Debug"
        defines "BG_DBG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "BG_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "BG_DIST"
        runtime "Release"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    staticruntime "off"

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
        systemversion "latest"

        defines 
        {
            "BG_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "BG_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "BG_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "BG_DIST"
        runtime "Release"
        optimize "On"