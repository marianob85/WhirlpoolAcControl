cmake_minimum_required(VERSION 3.20)

# Set the project name
project (Whirlpool_YJ1B LANGUAGES CXX )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

add_compile_definitions(TRACE)

add_subdirectory( external )
add_subdirectory( src )
enable_testing()
add_subdirectory( tests )
