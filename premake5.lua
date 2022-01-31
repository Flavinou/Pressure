workspace "Pressure"
    architecture "x64"
    startproject "Sandbox"

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

include "Pressure/extern/GLFW"
include "Pressure/extern/Glad"
include "Pressure/extern/imgui"

project "Pressure"
    location "Pressure"
    kind "SharedLib"
    language "C++"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "prspch.h"
    pchsource "Pressure/src/prspch.cpp"
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
		"%{prj.name}/extern/glm/glm/**.hpp",
		"%{prj.name}/extern/glm/glm/**.inl",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/extern/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
    }
    
    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

        defines
        {
            "PRS_PLATFORM_WINDOWS",
            "PRS_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
        }

    filter "configurations:Debug"
        defines "PRS_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "PRS_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "PRS_DIST"
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
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "Pressure/extern/spdlog/include",
        "Pressure/src",
        "Pressure/extern",
        "%{IncludeDir.glm}"
    }

    links
    {
        "Pressure"
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

        defines
        {
            "PRS_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "PRS_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "PRS_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "PRS_DIST"
        runtime "Release"
        optimize "On"