include "./extern/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Pressure"
    architecture "x64"
    startproject "Sigil"

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
	include "extern/premake"
	include "Pressure/extern/GLFW"
	include "Pressure/extern/Glad"
	include "Pressure/extern/imgui"
	include "Pressure/extern/yaml-cpp"

group ""

include "Pressure"
include "Sandbox"
include "Sigil"
