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

WhirlpoolYJ1B& WhirlpoolYJ1B::setMode( Mode mode )
{
	const auto data = tounderlying( mode );
	if( data > 4 )
		m_data.bits.bit0.mode = 0;
	m_data.bits.bit0.mode = data;
}

Mode WhirlpoolYJ1B::getMode() const
{
	return static_cast< Mode >( m_data.bits.bit0.mode );
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setPower( bool power )
{
	m_data.bits.bit0.power = power;
	return *this;
}

bool WhirlpoolYJ1B::getPower() const
{
	return m_data.bits.bit0.power;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setFan( Fan fan )
{
	m_data.bits.bit0.fan = tounderlying( fan );
	return *this;
}

Fan WhirlpoolYJ1B::getFan() const
{
	return static_cast< Fan >( m_data.bits.bit0.fan );
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setSwing( bool swing )
{
	m_data.bits.bit0.swing = swing;
	return *this;
}

bool WhirlpoolYJ1B::getSwing() const
{
	return m_data.bits.bit0.swing;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setSleep( bool sleep )
{
	m_data.bits.bit0.sleep = sleep;
	return *this;
}

bool WhirlpoolYJ1B::getSleep() const
{
	return m_data.bits.bit0.sleep;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setJet( bool jet )
{
	m_data.bits.bit6.jet = jet;
	return *this;
}

bool WhirlpoolYJ1B::getJet() const
{
	return m_data.bits.bit6.jet;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setLight( bool light )
{
	m_data.bits.bit6.light = light;
	return *this;
}

bool WhirlpoolYJ1B::getLight() const
{
	return m_data.bits.bit6.light;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setTemperatureRaw( uint8_t temp )
{
	m_data.bits.bit1.temperature = temp > 14 ? 14 : temp;
	return *this;
}

uint8_t WhirlpoolYJ1B::getTemperatureRaw() const
{
	return m_data.bits.bit1.temperature;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setTemperature( uint8_t temp )
{
	if( temp > 30 )
		m_data.bits.bit1.temperature = 30 - 16;
	else if( temp < 16 )
		m_data.bits.bit1.temperature = 0;
	else
		m_data.bits.bit1.temperature = temp - 16;

		return *this;
}

uint8_t WhirlpoolYJ1B::getTemperature() const
{
	return m_data.bits.bit1.temperature + 16;
}
