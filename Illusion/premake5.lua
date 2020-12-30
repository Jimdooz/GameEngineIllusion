workspace "Illusion"
	architecture "x86_64"
	startproject "IllusionEditor"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories 
IncludeDir = {}
IncludeDir["glfw"] = "%{wks.location}/libs/glfw/include"
IncludeDir["Glad"] = "%{wks.location}/libs/Glad/include"
IncludeDir["glm"] = "%{wks.location}/libs/glm/"
IncludeDir["imgui"] = "%{wks.location}/libs/imgui/"
IncludeDir["assimp"] = "%{wks.location}/libs/assimp/include"
IncludeDir["IllusionEngine"] = "%{wks.location}/IllusionEngine/include"
project "IllusionEngine"
	location "%{wks.location}/%{prj.name}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("build/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	-- pchheader "ILSEngine_pch.h"
	-- pchsource "%{prj.name}/src/ILSEngine_pch.cpp"

	files
	{
		"%{prj.location}/include/**.h",
		"%{prj.location}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.location}/include/",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.Glad}"
	}

	filter "system:linux"
		systemversion "latest"
		defines
		{
			"ILS_PLATFORM_LINUX"
		}
		--GLad files
		files
		{
			"%{wks.location}/libs/Glad/include/glad/glad.h",
			"%{wks.location}/libs/Glad/include/KHR/khrplatform.h",
			"%{wks.location}/libs/Glad/src/glad.c"
		}
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ILS_PLATFORM_WINDOWS"
		}

		links
		{
			"Glad",
			"libs/glfw/lib-vc2019/glfw3.lib"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../build/" .. outputdir .. "/IllusionEditor/\"")
		}

	filter "configurations:Debug"
		defines {
			"ILSENGINE_DEBUG",
			"ILSENGINE_INTERNAL_DEBUG"
		}
		runtime "Debug"
		symbols "On"
		optimize "Off"

	filter "configurations:Release"
		defines "ILSENGINE_RELEASE"
		runtime "Release"
		optimize "On"
project "IllusionEditor"
	location "%{wks.location}/%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("build/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		--ImGui files
		"%{wks.location}/libs/imgui/imconfig.h",
		"%{wks.location}/libs/imgui/imgui.h",
		"%{wks.location}/libs/imgui/imgui_internal.h",
		"%{wks.location}/libs/imgui/imstb_rectpack.h",
		"%{wks.location}/libs/imgui/imstb_textedit.h",
		"%{wks.location}/libs/imgui/imstb_truetype.h",
		"%{wks.location}/libs/imgui/imgui_impl_opengl3.h",
		"%{wks.location}/libs/imgui/imgui_impl_glfw.h",
		"%{wks.location}/libs/imgui/imgui.cpp",
		"%{wks.location}/libs/imgui/imgui_draw.cpp",
		"%{wks.location}/libs/imgui/imgui_widgets.cpp",
		"%{wks.location}/libs/imgui/imgui_impl_opengl3.cpp",
		"%{wks.location}/libs/imgui/imgui_impl_glfw.cpp",
		"%{wks.location}/libs/imgui/imgui_demo.cpp",

		"%{prj.location}/include/**.h",
		"%{prj.location}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.location}/include",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.IllusionEngine}"
	}

	filter "system:linux"
		systemversion "latest"

		defines
		{
			"ILS_PLATFORM_LINUX"
		}
		libdirs 
		{
			"%{wks.location}/libs/glfw/liblinux/",
			"%{wks.location}/libs/assimp/liblinux/"
		}
		links
		{
			--GLFW dependencies
			"glfw3",
			"Xrandr",
			"Xi",
			"GLU",
			"GL",
			"X11",
			"dl",
			"pthread",
			"stdc++fs",
			
			--ASSIMP Dependencies
			"assimp",
			"IrrXML",
			"z",

			"IllusionEngine"
		}
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ILS_PLATFORM_WINDOWS"
		}
		links
		{
			"IllusionEngine"
		}
	filter "configurations:Debug"
		defines {
			"ILSENGINE_DEBUG",
			"ILSENGINE_INTERNAL_DEBUG"
		}
		runtime "Debug"
		symbols "On"
		optimize "Off"

	filter "configurations:Release"
		defines "ILS_RELEASE"
		runtime "Release"
		optimize "On"
project "Glad"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	location "%{wks.location}/libs/%{prj.name}/"
	
	targetdir ("build/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")
	
	filter "system:windows"
		systemversion "latest"
		files
		{
			"%{prj.location}/include/glad/glad.h",
			"%{prj.location}/include/KHR/khrplatform.h",
			"%{prj.location}/src/glad.c"
		}

		includedirs
		{
			"%{prj.location}/include"
		}
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		optimize "off"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"