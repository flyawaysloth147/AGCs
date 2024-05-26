project "Glad"
        kind "staticlib"
        language "C++"
        cppdialect "C++20"
        staticruntime "off"

        targetdir ("%{wks.location}/bin/" .. outDir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outDir .. "/%{prj.name}")

        files {
            "src/**.c",
            "include/**.h"
        }

        includedirs {
            "%{prj.location}/include"
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