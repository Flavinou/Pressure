project "Pressure"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "prspch.h"
    pchsource "Pressure/src/prspch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",
        "extern/stb_image/**.h",
        "extern/stb_image/**.cpp",
        "extern/glm/glm/**.hpp",
        "extern/glm/glm/**.inl",
    }

    defines
    {
        "_CRT_NO_SECURE_WARNINGS",
 		"GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "src",
        "extern/spdlog/include",
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
