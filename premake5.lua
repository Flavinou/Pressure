include "./extern/premake/premake_customization/solution_items.lua"

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

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Pressure/extern/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Pressure/extern/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Pressure/extern/imgui"
IncludeDir["glm"] = "%{wks.location}/Pressure/extern/glm"
IncludeDir["stb_image"] = "%{wks.location}/Pressure/extern/stb_image"
IncludeDir["entt"] = "%{wks.location}/Pressure/extern/entt/include"

include "Pressure/extern/GLFW"
include "Pressure/extern/Glad"
include "Pressure/extern/imgui"

group "Dependencies"
	include "extern/premake"
	include "Pressure/extern/GLFW"
	include "Pressure/extern/Glad"
	include "Pressure/extern/imgui"

group ""

include "Pressure"
include "Sandbox"
include "Sigil"