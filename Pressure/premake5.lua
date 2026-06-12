project "Pressure"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "prspch.h"
    pchsource "src/prspch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",
        "extern/stb_image/**.h",
        "extern/stb_image/**.cpp",
        "extern/glm/glm/**.hpp",
        "extern/glm/glm/**.inl",
		"extern/ImGuizmo/ImGuizmo.h",
		"extern/ImGuizmo/ImGuizmo.cpp"
    }

    defines
    {
        "_CRT_NO_SECURE_WARNINGS",
 		"GLFW_INCLUDE_NONE",
		"_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING",
		"YAML_CPP_STATIC_DEFINE"
    }

    includedirs
    {
        "src",
        "extern/spdlog/include",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.filewatch}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.mono}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.VulkanSDK}"
    }

    links
    {
		"Box2D",
        "GLFW",
        "Glad",
        "ImGui",
        "msdf-atlas-gen",
		"yaml-cpp",
        "opengl32.lib",
		"%{Library.mono}",
    }

	filter "files:extern/ImGuizmo/**.cpp"
	flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"

        defines
        {
        }

		links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}",
		}

    filter "configurations:Debug"
        defines "PRS_DEBUG"
        runtime "Debug"
        symbols "on"

		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

    filter "configurations:Release"
        defines "PRS_RELEASE"
        runtime "Release"
        optimize "on"

		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

    filter "configurations:Dist"
        defines "PRS_DIST"
        runtime "Release"
        optimize "on"

		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}
