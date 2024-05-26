workspace "Axantera Ground Control Software"
    architecture "x64"
    configurations {
        "Debug",
        "Release",
        "Distribute"
    }

    startproject "AGC"

    outDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    include "AGC/Vendor/GLAD"

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
            "%{prj.name}/Src/**.cpp",
            "%{prj.name}/Src/**.hpp",
            "%{prj.name}/Src/**.h",
            "%{prj.name}/Src/**.c"
        }

        includedirs {
            "%{prj.name}/Src/",
            "%{prj.name}/Vendor/SPDLOG/include",
            "%{prj.name}/Vendor/GLFW/include",
            "%{prj.name}/Vendor/Glad/include"
        }

        links {
            "%{prj.location}/Vendor/GLFW/lib-vc2022/glfw3_mt.lib",
            "Glad"
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


