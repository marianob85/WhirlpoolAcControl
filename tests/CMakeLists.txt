project(Whirlpool_YJ1BTests LANGUAGES CXX )

set( SOURCES
    Whirlpool_YJ1BTest.cpp
) 

add_executable( Whirlpool_YJ1BTests  ${SOURCES} )

target_link_libraries(Whirlpool_YJ1BTests Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests( Whirlpool_YJ1BTests ) 