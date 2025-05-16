project "Sigil"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

	defines
	{
		"_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING"
	}

    includedirs
    {
        "%{wks.location}/Pressure/extern/spdlog/include",
        "%{wks.location}/Pressure/src",
        "%{wks.location}/Pressure/extern",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGuizmo}"
    }

    links
    {
        "Pressure"
    }

    filter "system:windows"
        systemversion "latest"

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
