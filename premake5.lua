workspace "Axantera Ground Control Software"
    architecture "x64"
    configurations {
        "Debug",
        "Release",
        "Distribute"
    }

    startproject "AGC"

    outDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    include "AGC/Vendor"

    project "AGC"
        location "AGC"
        kind "consoleapp"
        language "C++"
        cppdialect "C++20"
        staticruntime "off"

        pchheader "AGCpch.h"
        pchsource "AGC/Src/AGCpch.cpp"

        targetdir ("%{wks.location}/bin/" .. outDir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outDir .. "/%{prj.name}")

        files {
            "%{prj.location}/Src/**.cpp",
            "%{prj.location}/Src/**.hpp",
            "%{prj.location}/Src/**.h",
            "%{prj.location}/Src/**.c"
        }

        includedirs {
            "%{prj.location}/Src/",
            "%{prj.location}/Vendor/SPDLOG/include",
            "%{prj.location}/Vendor/GLFW/include",
            "%{prj.location}/Vendor/Glad/include",
            "%{prj.location}/Vendor/GLM",
            "%{prj.location}/Vendor/IMGUI",
            "%{prj.location}/Vendor/IMPLOT"
        }

        links {
            "%{prj.location}/Vendor/GLFW/lib-vc2022/glfw3_mt.lib",
            "Glad",
            "ImGui",
            "ImPlot"
        }

        filter "system:windows"
            defines{
                "BUILD_WINDOWS"
            }

        systemversion "latest"

        filter "configurations:Debug"
            defines{
                "BUILD_DEBUG"
            }

            runtime "debug"
            symbols "on"
            optimize "off"

        filter "configurations:Release"
            defines{
                "BUILD_RELEASE"
            }

            runtime "release"
            symbols "on"
            optimize "on"

        filter "configurations:Distribute"
            defines{
                "BUILD_DIST"
            }

            runtime "release"
            symbols "off"
            optimize "on"


