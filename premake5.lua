include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Bubble"
	architecture "x86_64"
	startproject "Bubblegum"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	--include "Bubble/vendor/Box2D"
	include "Bubble/vendor/GLFW"
	include "Bubble/vendor/Glad"
	--include "Bubble/vendor/msdf-atlas-gen"
	include "Bubble/vendor/imgui"
	include "Bubble/vendor/yaml-cpp"
group ""

group "Core"
	include "Bubble"
	--include "Bubble-ScriptCore"
group ""

group "Tools"
	include "Bubblegum"
group ""

group "Misc"
	include "Sandbox"
group ""