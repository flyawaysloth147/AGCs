project "Glad"
        location "GLAD"
        kind "staticlib"
        language "C++"
        cppdialect "C++20"
        staticruntime "off"

        targetdir ("%{wks.location}/bin/" .. outDir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outDir .. "/%{prj.name}")

        files {
            "%{prj.location}/src/**.c",
            "%{prj.location}/include/**.h"
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

project "ImGui"
    location "IMGUI"
    kind "staticlib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outDir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outDir .. "/%{prj.name}")

    files {
        "%{prj.location}/*.cpp",
        "%{prj.location}/*.h",
        "%{prj.location}/backends/imgui_impl_glfw.cpp",
        "%{prj.location}/backends/imgui_impl_glfw.h",
        "%{prj.location}/backends/imgui_impl_opengl3.cpp",
        "%{prj.location}/backends/imgui_impl_opengl3.h"
    }

    includedirs {
        "%{prj.location}/",
        "GLFW/include"
    }

    links {
        "GLFW/lib-vc2022/glfw3_mt.lib"
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