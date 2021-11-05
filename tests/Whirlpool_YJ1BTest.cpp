#include <catch2/catch.hpp>

TEST_CASE( "Get uninitialized value", "[Value holder]" )
{
	const auto i = 10;
	REQUIRE( i == 10 );
}
