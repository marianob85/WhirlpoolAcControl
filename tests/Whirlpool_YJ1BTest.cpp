#include <catch2/catch.hpp>
#include <Whirlpool_YJ1B.hpp>

using namespace std;

template< int N >
class Compare
{
public:
	Compare( const typename WhirlpoolYJ1B< N >::Data& data ) : m_data{ data } {};

	bool operator()( std::initializer_list< uint8_t > data, uint16_t size ) const
	{
		const vector< uint8_t > vdata( data );
		if( vdata.size() != m_data.storageSize )
			return false;
		if( size != m_data.dataSize )
			return false;

		for( unsigned i = 0; i < m_data.storageSize; ++i )
		{
			uint8_t mask = 0xFF;
			if( const auto bits = m_data.dataSize / ( ( i + 1 ) * 8 ); bits == 0 )
				mask = ~( mask << ( 8 - ( m_data.dataSize % 8 ) ) );
			if( ( vdata.at( i ) & mask ) != m_data.data[ i ] )
				return false;
		}

		return true;
	}

private:
	const typename WhirlpoolYJ1B< N >::Data& m_data{};
};

TEST_CASE( "Default value" )
{
	constexpr uint8_t size = 24;
	const WhirlpoolYJ1B< size > test;
	REQUIRE( Compare< size >( test.data() )( { 0, 0, 0 }, size ) );
}

TEST_CASE( "Custom value" )
{
	SECTION( "Fitted data" )
	{
		constexpr uint8_t size = 24;
		const WhirlpoolYJ1B< size >::Data data{ 1, 2, 3 };
		const WhirlpoolYJ1B< size > test( data );
		REQUIRE( Compare< size >( test.data() )( { 1, 2, 3 }, size ) );
	}
	SECTION( "Un-fitted data" )
	{
		constexpr uint8_t size = 22;
		const WhirlpoolYJ1B< size >::Data data{ 1, 2, 3 };
		const WhirlpoolYJ1B< size > test( data );
		REQUIRE( Compare< size >( test.data() )( { 1, 2, 0xFF }, size ) );
	}
}