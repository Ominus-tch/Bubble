project "BubbleGum"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Bubble/vendor/spdlog/include",
		"%{wks.location}/Bubble/src",
		"%{wks.location}/Bubble/vendor",
		"%{IncludeDir.entt}",
		--"%{IncludeDir.filewatch}",
		"%{IncludeDir.glm}",
		--"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"Bubble"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "BG_DEBUG"
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