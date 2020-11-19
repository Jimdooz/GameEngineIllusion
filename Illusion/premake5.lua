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
		"%{IncludeDir.Glad}"
	}

	links 
	{
		"Glad"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ILS_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../build/" .. outputdir .. "/IllusionEditor/\"")
		}

		links
		{
			"libs/glfw/lib-vc2019/glfw3.lib"
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
		"%{prj.location}/include/**.h",
		"%{prj.location}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.location}/include",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.IllusionEngine}"
	}

	links
	{
		"IllusionEngine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ILS_PLATFORM_WINDOWS"
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
	
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		optimize "off"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
	