project "FloofySharp"
    kind "StaticLib"
    language "C++"
    
    targetdir ("bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}-%{cfg.startproject}/%{prj.name}")
    objdir ("bin-obj/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}-%{cfg.startproject}/%{prj.name}")
    
    
    files 
    {
        "**.h",
        "**.c",
        "**.hpp",
        "**.cpp",
    }

    includedirs
    {
        "FloofySharp"
    }
    
    flags
    {
        "NoRuntimeChecks",
        "MultiProcessorCompile"
    }
    
    --platforms
    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"
    
        defines
        {
            "Window_Build",
            "Desktop_Build"
        }
    
    --configs
    filter "configurations:Debug"
        defines "BTD_DEBUG"
        symbols "On"
    
    filter "configurations:Release"
        defines "BTD_RELEASE"
        optimize "On"
    
    filter "configurations:Dist"
        defines "BTD_DIST"
        optimize "On"
    
        defines
        {
            "NDEBUG"
        }
    
        flags
        {
           "LinkTimeOptimization"
        }