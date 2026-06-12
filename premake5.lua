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
	include "Pressure/extern/box2d"
	include "Pressure/extern/GLFW"
	include "Pressure/extern/Glad"
	include "Pressure/extern/imgui"
	include "Pressure/extern/msdf-atlas-gen"
	include "Pressure/extern/yaml-cpp"
group ""

group "Core"
	include "Pressure"
	include "Pressure-ScriptCore"
group ""

group "Tools"
	include "Sigil"
group ""

group "Misc"
	include "Sandbox"
group ""
