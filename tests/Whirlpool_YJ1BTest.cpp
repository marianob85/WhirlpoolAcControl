#include <catch2/catch.hpp>
#include <Whirlpool_YJ1B.hpp>

using namespace std;

TEST_CASE( "Default value" )
{
	auto data = WhirlpoolYJ1BData();

	const vector< uint8_t > expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	REQUIRE( std::equal( std::begin( data.raw ), std::end( data.raw ), expected.begin() ) );
}

TEST_CASE( "Test Sleep" )
{
	auto data = WhirlpoolYJ1B();
	data.setSleep( true );
	const vector< uint8_t > expected = { 0b10000000, 0, 0, 0, 0, 0, 0, 0, 0 };
	REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expected.begin() ) );
}

TEST_CASE( "Test Mode" )
{
	auto data = WhirlpoolYJ1B();
	data.setMode( Mode::Sense_6th );
	const vector< uint8_t > expected = { 0b00000000, 0, 0, 0, 0, 0, 0, 0, 0 };
	REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expected.begin() ) );
	data.setMode( Mode::Heat );
	const vector< uint8_t > expectedA = { 0b00000100, 0, 0, 0, 0, 0, 0, 0, 0 };
	REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expectedA.begin() ) );
}

TEST_CASE( "Test Temperature" )
{
	auto data = WhirlpoolYJ1B();

	const vector< uint8_t > expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expected.begin() ) );

	SECTION( "Under min" )
	{
		data.setTemperature( 0 );

		const vector< uint8_t > expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expected.begin() ) );
	}

	SECTION( "Min" )
	{
		data.setTemperature( 16 );

		const vector< uint8_t > expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expected.begin() ) );
	}

	SECTION( "Middle ( 20 )" )
	{
		data.setTemperature( 20 );

		const vector< uint8_t > expected = { 0, 0b00000100, 0, 0, 0, 0, 0, 0, 0 };
		REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expected.begin() ) );
	}

	SECTION( "Max" )
	{
		data.setTemperature( 30 );

		const vector< uint8_t > expected = { 0, 0b00001110, 0, 0, 0, 0, 0, 0, 0 };
		REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expected.begin() ) );
	}

	SECTION( "Over Max" )
	{
		data.setTemperature( 30 );

		const vector< uint8_t > expected = { 0, 0b00001110, 0, 0, 0, 0, 0, 0, 0 };
		REQUIRE( std::equal( std::begin( data.data().raw ), std::end( data.data().raw ), expected.begin() ) );
	}
}
