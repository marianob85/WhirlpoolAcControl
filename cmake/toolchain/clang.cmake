include_guard()

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )
set( CMAKE_CXX_VISIBILITY_PRESET hidden )
set( CMAKE_VISIBILITY_INLINES_HIDDEN ON )

add_compile_options(
    -Wall
	-Wno-invalid-offsetof # NuBase
	-Wno-unknown-pragmas # too many Windows specific pragmas when includes the headers
	-Wno-missing-braces
	-Wno-overloaded-virtual
	-Wno-reorder
	-Wno-constant-conversion
	-Wno-inconsistent-missing-override # NuBase
	-Wno-format-security # NuBase
	-Wno-delete-non-virtual-dtor # NuBase
	-Wno-unused-variable # NuBase
	-fdeclspec
	-Wno-macro-redefined
)
if( UNIX )
	add_link_options( "LINKER:-z,defs" )
endif()