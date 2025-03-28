workspace "Pressure"
    architecture "x64"
    startproject "Sigil"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Pressure/extern/GLFW/include"
IncludeDir["Glad"] = "Pressure/extern/Glad/include"
IncludeDir["ImGui"] = "Pressure/extern/imgui"
IncludeDir["glm"] = "Pressure/extern/glm"
IncludeDir["stb_image"] = "Pressure/extern/stb_image"
IncludeDir["entt"] = "Pressure/extern/entt/include"

include "Pressure/extern/GLFW"
include "Pressure/extern/Glad"
include "Pressure/extern/imgui"

group "Dependencies"
	include "Pressure/extern/GLFW"
	include "Pressure/extern/Glad"
	include "Pressure/extern/imgui"

group ""

project "Pressure"
    location "Pressure"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "prspch.h"
    pchsource "Pressure/src/prspch.cpp"
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/extern/stb_image/**.h",
        "%{prj.name}/extern/stb_image/**.cpp",
        "%{prj.name}/extern/glm/glm/**.hpp",
        "%{prj.name}/extern/glm/glm/**.inl",
    }

    defines
    {
        "_CRT_NO_SECURE_WARNINGS",
 		"GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/extern/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}"
    }
    
    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "PRS_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "PRS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "PRS_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "PRS_DIST"
        runtime "Release"
        optimize "on"


project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")    
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "Pressure/extern/spdlog/include",
        "Pressure/src",
        "Pressure/extern",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "Pressure"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "PRS_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "PRS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "PRS_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "PRS_DIST"
        runtime "Release"
        optimize "on"


project "Sigil"
    location "Sigil"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")    
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "Pressure/extern/spdlog/include",
        "Pressure/src",
        "Pressure/extern",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "Pressure"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "PRS_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "PRS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "PRS_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "PRS_DIST"
        runtime "Release"
        optimize "on"