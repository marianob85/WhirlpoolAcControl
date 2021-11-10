#include "Whirlpool_YJ1B.hpp"
#include <type_traits>

using namespace std;

template< typename E >
constexpr typename std::underlying_type< E >::type tounderlying( E e ) noexcept
{
	return static_cast< typename std::underlying_type< E >::type >( e );
}

WhirlpoolYJ1B::WhirlpoolYJ1B( const WhirlpoolYJ1BData& data ) : m_data{ data } {}

WhirlpoolYJ1BData& WhirlpoolYJ1B::data()
{
	return m_data;
}

void WhirlpoolYJ1B::setMode( Mode mode )
{
	m_data.bit0.mode = tounderlying( mode );
}

void WhirlpoolYJ1B::setPower( bool power ) {}

void WhirlpoolYJ1B::setFan( Fan fan ) {}

void WhirlpoolYJ1B::setSwitg( bool swing ) {}

void WhirlpoolYJ1B::setSleep( bool sleep ) {}

#ifdef WhirlpoolYJ1B_TRACE
void WhirlpoolYJ1B::print() {}
#endif