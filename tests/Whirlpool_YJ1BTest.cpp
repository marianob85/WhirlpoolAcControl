#include <catch2/catch.hpp>
#include <Whirlpool_YJ1B.hpp>

TEST_CASE( "Get uninitialized value", "[Value holder]" )
{

	WhirlpoolYJ1B< 10 > test;

	const auto i = 10;
	REQUIRE( i == 10 );
}
