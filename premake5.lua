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
IncludeDir["glm"] = "Bubble/vendor/glm"

include "Bubble/vendor/GLFW"
include "Bubble/vendor/Glad"
include "Bubble/vendor/imgui"

project "Bubble"
    location "Bubble"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "bgpch.h"
    pchsource "Bubble/src/bgpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}"
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
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"

        defines 
        {
            "BG_PLATFORM_WINDOWS",
            "BG_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "BG_DBG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "BG_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "BG_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
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
        "Bubble/vendor/spdlog/include",
        "Bubble/src",
        "Bubble/vendor",
        "%{IncludeDir.glm}"
    }

    links
    {
        "Bubble"
    }

    filter "system:windows"
        systemversion "latest"

        defines 
        {
            "BG_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "BG_DBG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "BG_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "BG_DIST"
        runtime "Release"
        optimize "on"
