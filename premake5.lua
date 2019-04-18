workspace "Haze"
	architecture "x64"
	startproject "Sandbox"

	configurations 
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Haze/vendor/GLFW/include"
IncludeDir["Glad"] = "Haze/vendor/Glad/include"
IncludeDir["ImGui"] = "Haze/vendor/imgui"
IncludeDir["glm"] = "Haze/vendor/glm"
IncludeDir["assimp"] = "Haze/vendor/assimp/include"
IncludeDir["assimp_cfg"] = "Haze/vendor/assimp/build/include"

group "Dependencies"
	include "Haze/vendor/GLFW"
	include "Haze/vendor/Glad"
	include "Haze/vendor/imgui"
group ""

project "Haze"
	location "Haze"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hzpch.h"
	pchsource "Haze/src/hzpch.cpp"

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.assimp_cfg}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"%{prj.name}/../vendor/assimp/build/contrib/irrXML/%{cfg.buildcfg}/IrrXML.lib",
		"%{prj.name}/../vendor/assimp/build/code/%{cfg.buildcfg}/assimp-vc140-mt.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HZ_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "on"

		links 
		{
			"%{prj.name}/../vendor/assimp/build/contrib/zlib/%{cfg.buildcfg}/zlibstaticd.lib"
		}

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"

		links 
		{
			"%{prj.name}/../vendor/assimp/build/contrib/zlib/%{cfg.buildcfg}/zlibstatic.lib"
		}

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"

		links 
		{
			"%{prj.name}/../vendor/assimp/build/contrib/zlib/%{cfg.buildcfg}/zlibstatic.lib"
		}

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Haze/vendor/spdlog/include",
		"Haze/src",
		"Haze/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Haze"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HZ_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"
