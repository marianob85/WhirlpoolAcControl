add_library( Whirlpool_YJ1B STATIC )


target_include_directories( Whirlpool_YJ1B INTERFACE
    ./Whirlpool_YJ1B
)

target_sources( Whirlpool_YJ1B PRIVATE
    Whirlpool_YJ1B/Whirlpool_YJ1B.hpp
    Whirlpool_YJ1B/Whirlpool_YJ1B.cpp
)
