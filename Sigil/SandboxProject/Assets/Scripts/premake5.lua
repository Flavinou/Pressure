local PressureRootDir = '../../../..'
include (PressureRootDir .. '/extern/premake/premake_customization/solution_items.lua')

workspace "Sandbox"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Sandbox"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("bin")
	objdir ("bin-int")

	files 
	{
		"src/**.cs",
		"Properties/**.cs"
	}

	links
	{
		"Pressure-ScriptCore"
	}
	
	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"

group "Pressure"
	include (PressureRootDir .. '/Pressure-ScriptCore')
group ""
