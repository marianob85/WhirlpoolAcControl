project(Whirlpool_YJ1BTests LANGUAGES CXX )

set( SOURCES
    main.cpp
    Whirlpool_YJ1BTest.cpp
) 

add_executable( Whirlpool_YJ1BTests ${SOURCES} )

target_link_libraries(Whirlpool_YJ1BTests Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests( Whirlpool_YJ1BTests ) 