add_library( Whirlpool_YJ1B STATIC )


target_include_directories( Whirlpool_YJ1B INTERFACE
    ./
)

target_sources( Whirlpool_YJ1B PRIVATE
    Whirlpool_YJ1B.hpp
    Whirlpool_YJ1B.cpp

    Whirlpool_YJ1B_Remote.hpp
    Whirlpool_YJ1B_Receive.hpp
    Whirlpool_YJ1B_Receive.cpp
)
