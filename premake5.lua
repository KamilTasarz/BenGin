-- To-Do: Change %{cfg.buildtarget.abspath} to relativepath (Cherno #7 Premake)

workspace "BenGin" -- Solution Name
	
	architecture "x64" -- Platform (64 bit)
	
	configurations {
		"Debug", -- slower debug
		"Release", -- faster debug
		"Dist" -- true release
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" -- created variable for bin path location

includedir = "%{prj.name}/src/" -- created variable for files included into project
includedep = "%{prj.name}/dependencies/"

project "BenGin" -- Engine Project Name
	
	location "BenGin" -- directory of the project files
	
	kind "SharedLib" -- build into DLL
	language "C++" -- language

	targetdir ("bin/" .. outputdir .. "/%{prj.name}") -- directory for binaries, exe, dll
	objdir ("bin-intermediate/" .. outputdir .. "/%{prj.name}") -- directory for intermediates, obj, etc.

	files {
		includedir .. "**.h",
		includedir .. "**.cpp",
		
		"%{prj.name}/dependencies/glad/glad.c",
		"%{prj.name}/dependencies/glad/glad.h"
		
		-- includedep .. "**.h",
		-- includedep .. "**.hpp",
		-- includedep .. "**.c",
		-- includedep .. "**.cpp"
	}

	includedirs {
		"%{prj.name}/dependencies/spdlog/include",
		-- "%{prj.name}/dependencies/spdlog",
		"%{prj.name}/dependencies/assimp",
		"%{prj.name}/dependencies/glad",
		"%{prj.name}/dependencies"
	}

	links {
		"assimp-vc143-mtd.dll",
		"assimp-vc143-mtd.lib",
		"fmodL.dll",
		"fmodL_vc.lib",
		"fmodstudioL.dll",
		"fmodstudioL_vc.lib",
		"freetype.dll",
		"freetype.lib",
		"glfw3.lib"
	}

	libdirs {
		"%{prj.name}",
		"%{prj.name}/dependencies/lib"
	}

	filter "system:windows" -- this is only in case if the system is Windows

		cppdialect "C++20"
		staticruntime "On" -- linking runtime libraries
		systemversion "latest" -- automatically generates for an old Windows SDK
		
		buildoptions {
			"/utf-8"
		}

		defines {		   -- our macros (used in Core.h to determine if dllimport or export)
			"BENGIN",
			"BENGIN_BUILD_DLL"
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/BenDitor/\"")
		}

	filter "configurations:Debug"

		defines "BENGIN_DEBUG"
		symbols "On" -- debug symbols

	filter "configurations:Release"

		defines "BENGIN_RELEASE"
		optimize "On" -- release optimization

	filter "configurations:Dist"

		defines "BENGIN_DIST"
		optimize "On" -- release optimization

project "BenDitor" -- editor project name
	
	location "BenDitor" -- directory of the project files

	kind "ConsoleApp" -- build into EXE
	language "C++" -- language

	targetdir ("bin/" .. outputdir .. "/%{prj.name}") -- directory for binaries, exe, dll
	objdir ("bin-intermediate/" .. outputdir .. "/%{prj.name}") -- directory for intermediates, obj, etc.

	files {
		includedir .. "**.h",
		includedir .. "**.cpp"
	}

	includedirs {
		"BenGin/dependencies/spdlog/include",
		"BenGin/dependencies",
		"BenGin/src"
	}

	links {
		"BenGin" -- link our engine so that we can reference the other project
	}

	filter "system:windows" -- this is only in case if the system is Windows

		cppdialect "C++20"
		staticruntime "On" -- linking runtime libraries
		systemversion "latest" -- automatically generates for an old Windows SDK

		buildoptions {
			"/utf-8"
		}

		defines {		   -- our macros (used in Core.h to determine if dllimport or export)
			"BENGIN"
		}

	filter "configurations:Debug"

		defines "BENGIN_DEBUG"
		symbols "On" -- debug symbols

	filter "configurations:Release"

		defines "BENGIN_RELEASE"
		optimize "On" -- release optimization

	filter "configurations:Dist"

		defines "BENGIN_DIST"
		optimize "On" -- release optimization
