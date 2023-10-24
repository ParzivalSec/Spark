workspace "Spark"
    location("workspaces/" .. _ACTION) -- create a unique dir for every different premake action (VS17, VS13, ...) 
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64" }
    -- platform names have no meaning on their own - need filter context fot this

    filter { "platforms:Win32" }
        system "Windows"
        architecture "x32"

    filter { "platforms:Win64" }
        system "Windows"
        architecture "x64"
	filter {}
    
	flags { "ShadowedVariables", "UndefinedIdentifiers" }

-- During development let `UnitTests` be the startup project
project "UnitTests"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/unit_tests/%{cfg.buildcfg}"

    files { "src/UnitTests/**.h", "src/UnitTests/**.cpp" }

    links { "MemorySystem", "Core", "EntityComponentSystem" }
	
	filter { "platforms:Win64" }
		libdirs { "./ext/*/lib/x64/" }
	filter { "platforms:Win32" }
		libdirs { "./ext/*/lib/x86/" }
	filter {}
		
    -- google-test setup (includes/libs)
    includedirs { "ext/googletest/include", "src/MemorySystem/", "src/Containers", "src/Core", "src/ECS"}
    filter "configurations:Debug"
        symbols "On"
        links { "gtestd" }
        staticruntime "on"

    filter "configurations:Release"
	    symbols "Off"
        links { "gtest" }
        staticruntime "on"

-- Spark subsystems that will emit .lib files as artifacts
project "Core"
	kind "StaticLib"
	language "C++"
	targetname "core"
	targetdir "build/core/%{cfg.buildcfg}"
	
	files { "src/Core/**.h", "src/Core/**.cpp" }
	
	filter "configurations:Debug"
		symbols "On"
		staticruntime "on"

    filter "configurations:Release"
		symbols "Off"
        staticruntime "on"
		optimize "Full"
				
project "MemorySystem"
    kind "StaticLib"
    language "C++"
    targetname "mem-sys"
    targetdir "build/mem_sys/%{cfg.buildcfg}"

    files { "src/MemorySystem/**.h", "src/MemorySystem/**.cpp" }
	
	links { "Core" }
	includedirs { "src/Core/"}

    filter "configurations:Debug"
        symbols "On"
        staticruntime "on"

    filter "configurations:Release"
		symbols "Off"
        staticruntime "on"
		optimize "Full"

project "Containers"
    kind "StaticLib"
    language "C++"
    targetname "cont"
    targetdir "build/containers/%{cfg.buildcfg}"
	
	files { "src/Containers/**.h", "src/Containers/**.cpp" }
	
	-- Because Containers is a more high-level module of the engine
	-- it builds upon core modules as the MemorySystem and therefore
	-- linking it in here
	links { "MemorySystem", "Core" }
	includedirs { "src/MemorySystem/", "src/Core/"}

    filter "configurations:Debug"
        symbols "On"
        staticruntime "on"

    filter "configurations:Release"
	    symbols "Off"
        staticruntime "on"
		optimize "Full"

project "EntityComponentSystem"
    kind "StaticLib"
    language "C++"
    targetname "ecs"
    targetdir "build/ecs/%{cfg.buildcfg}"

	files { "src/ECS/**.h", "src/ECS/**.cpp" }
	
	links { "Core" }
	includedirs { "src/Core/"}

    filter "configurations:Debug"
        symbols "On"
        staticruntime "on"

    filter "configurations:Release"
	    symbols "Off"
        staticruntime "on"
		optimize "Full"

-- The `BenchmarkApplication` has to link against the .libs from it's siblings
-- The BenchmarkApplication will be able to run different test scenarios passed by cmd options
-- Each scenario is then measure for performance by another app (to ensure same clock for Spark++/RustySpark)
project "BenchmarkApplication"
    kind "ConsoleApp"
    language "C++"
	targetname "bench_spark++"
    targetdir "build/benchmark/%{cfg.buildcfg}"
	
	files { "src/BenchmarkApplication/**.h", "src/BenchmarkApplication/**.cpp" }

	links { "Core", "MemorySystem", "EntityComponentSystem"}
	includedirs { "src/Core/", "src/MemorySystem/", "src/Containers", "src/ECS"}
	
	filter "configurations:Debug"
		symbols "On"
		staticruntime "on"

    filter "configurations:Release"
	    symbols "Off"
        staticruntime "on"
		buildoptions { "/FAs" }
		optimize "Full"

-- Create a custom clean action
newaction {
    trigger = "clean",
    description = "clean the workspace folder",
    execute     = function () 
        print "Remove artifacts and ./workspaces/*"
        os.rmdir "./workspaces"
        os.rmdir "./build"
        print "Finished cleanup!"
    end
}
