#include "Whirlpool_YJ1B.hpp"

using namespace std;

#ifdef ARDUINO
template< typename T >
uint8_t tounderlying( T t )
{
	return static_cast< uint8_t >( t );
}
#else
#include <utility>
template< typename E >
constexpr typename std::underlying_type< E >::type tounderlying( E e ) noexcept
{
	return static_cast< typename std::underlying_type< E >::type >( e );
}
#endif

WhirlpoolYJ1B::WhirlpoolYJ1B( const WhirlpoolYJ1BData& data ) : m_data{ data } {}

WhirlpoolYJ1BData& WhirlpoolYJ1B::data()
{
	return m_data;
}

void WhirlpoolYJ1B::setMode( Mode mode )
{
	const auto data = tounderlying( mode );
	if( data > 4 )
		m_data.bits.bit0.mode = 0;
	m_data.bits.bit0.mode = data;
}

void WhirlpoolYJ1B::setPower( bool power )
{
	m_data.bits.bit0.power = power;
}

void WhirlpoolYJ1B::setFan( Fan fan )
{
	m_data.bits.bit0.fan = tounderlying( fan );
}

void WhirlpoolYJ1B::setSwitg( bool swing )
{
	m_data.bits.bit0.swing = swing;
}

void WhirlpoolYJ1B::setSleep( bool sleep )
{
	m_data.bits.bit0.sleep = sleep;
}

void WhirlpoolYJ1B::setJet( bool jet )
{
	m_data.bits.bit6.jet = jet;
}

void WhirlpoolYJ1B::setLight( bool light )
{
	m_data.bits.bit6.light = light;
}

void WhirlpoolYJ1B::setTemperature( uint8_t temp )
{
	if( temp > 30 )
		m_data.bits.bit1.temperature = 30 - 16;
	else if( temp < 16 )
		m_data.bits.bit1.temperature = 0;
	else
		m_data.bits.bit1.temperature = temp - 16;
}

#ifdef TRACE
void WhirlpoolYJ1B::print() {}
#endif