project "Glad"
        location "ProjectFile/%{prj.name}/"
        kind "staticlib"
        language "C++"
        cppdialect "C++20"
        staticruntime "off"

        targetdir ("%{wks.location}/bin/" .. outDir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outDir .. "/%{prj.name}")

        files {
            "GLAD/src/**.c",
            "GLAD/include/**.h"
        }

        includedirs {
            "GLAD/include"
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
    location "ProjectFile/%{prj.name}/"
    kind "staticlib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outDir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outDir .. "/%{prj.name}")

    files {
        "IMGUI/*.cpp",
        "IMGUI/*.h",
        "IMGUI/backends/imgui_impl_glfw.cpp",
        "IMGUI/backends/imgui_impl_glfw.h",
        "IMGUI/backends/imgui_impl_opengl3.cpp",
        "IMGUI/backends/imgui_impl_opengl3.h"
    }

    includedirs {
        "IMGUI/",
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

project "ImPlot"
    location "ProjectFile/%{prj.name}/"
    kind "staticlib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir ("%{wks.location}/bin/" .. outDir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outDir .. "/%{prj.name}")
    
    files {
        "IMPLOT/*.cpp",
        "IMPLOT/*.h"
    }
    
        includedirs {
            "IMPLOT/",
            "GLFW/include",
            "IMGUI/"
        }
    
        links {
            "GLFW/lib-vc2022/glfw3_mt.lib",
            "ImGui"
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