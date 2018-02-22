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

    flags { "FatalWarnings", "ShadowedVariables", "UndefinedIdentifiers" }

-- During development let `UnitTests` be the startup project
project "UnitTests"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/unit_tests/%{cfg.buildcfg}"

    files { "src/UnitTests/**.h", "src/UnitTests/**.cpp" }

    libdirs { "./ext/*/lib" }
    links { "MemorySystem" }
    
    -- google-test setup (includes/libs)
    includedirs { "ext/googletest/include", "src/MemorySystem/"}
    filter "configurations:Debug"
        symbols "On"
        links { "gtestd" }
        flags { "StaticRuntime" }

    filter "configurations:Release"
        symbols "On"
        links { "gtest" }
        flags { "StaticRuntime" }

-- Spark subsystems that will emit .lib files as artifacts
project "MemorySystem"
    kind "StaticLib"
    language "C++"
    targetname "mem-sys"
    targetdir "build/mem_sys/%{cfg.buildcfg}"

    files { "src/MemorySystem/**.h", "src/MemorySystem/**.cpp" }

    filter "configurations:Debug"
        symbols "On"
        flags { "StaticRuntime" }

    filter "configurations:Release"
        symbols "On"
        flags { "StaticRuntime" }

project "Containers"
    kind "StaticLib"
    language "C++"
    targetname "cont"
    targetdir "build/containers/%{cfg.buildcfg}"

project "EntityComponentSystem"
    kind "StaticLib"
    language "C++"
    targetname "ecs"
    targetdir "build/ecs/%{cfg.buildcfg}"

-- The `BenchmarkApplication` has to link against the .libs from it's siblings
project "BenchmarkApplication"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/benchmark/%{cfg.buildcfg}"

    -- links { "MemorySystem", "Containers", "EntityComponentSystem"  }

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
