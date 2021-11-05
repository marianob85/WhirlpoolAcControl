include_guard()

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10.0)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    /utf-8
    /W3
	/wd4828
	/wd4267

    /permissive-        # WinSDK must be at least 10.0.14393.0
    # These are not affected by /permissive- option:
    /Zc:inline           # Remove unreferenced function or data if it is COMDAT or has internal linkage only
    /Zc:throwingNew      # Assume operator new throws on failure.
	/MP
	/sdl
	/GS
	/FC
)

add_compile_definitions(
	#_UNICODE
	#UNICODE
)

set(CMAKE_CXX_FLAGS "/GR /EHsc"
    CACHE STRING "Flags used by the CXX compiler during all build types."
)

foreach(lang IN ITEMS C CXX)
    # /Gy - Allows the compiler to package individual functions in the form of packaged functions.
    # /Gw - Package global data in COMDAT sections for optimization.
    # /Oi - Replaces some function calls with intrinsic or otherwise special forms of the function that help your application run faster.
    # /JMC - Just My Code
    # NOTE: /Ob2 is in effect when /O1 or /O2 is used
    set(CMAKE_${lang}_FLAGS_DEBUG "/MDd /JMC /Zi /Od /RTC1"
        CACHE STRING "Flags used by the ${lang} compiler during DEBUG builds."
    )
    set(CMAKE_${lang}_FLAGS_RELEASE "/MD /O2 /Oi /DNDEBUG /Gw /Gy /Zi"
        CACHE STRING "Flags used by the ${lang} compiler during RELEASE builds."
    )
endforeach()

foreach(mod IN ITEMS EXE MODULE SHARED)
    set(CMAKE_${mod}_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL"
        CACHE STRING "Flags used by the linker during DEBUG builds."
    )
    set(CMAKE_${mod}_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /LTCG:incremental /OPT:REF /OPT:ICF"
        CACHE STRING "Flags used by the linker during RELEASE builds."
    )
endforeach()
