workspace "Pressure"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Pressure"
    location "Pressure"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/extern/spdlog/include",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "PRS_PLATFORM_WINDOWS",
            "PRS_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines "PRS_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "PRS_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "PRS_DIST"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

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
        "Pressure/src"
    }

    links
    {
        "Pressure"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "PRS_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "PRS_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "PRS_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "PRS_DIST"
        optimize "On"