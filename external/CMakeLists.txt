include(FetchContent)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set( CATCH_BUILD_STATIC_LIBRARY ON )

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.7
)

FetchContent_GetProperties(Catch2)
if(NOT catch2_POPULATED) 
    FetchContent_Populate(Catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib )
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
endif()