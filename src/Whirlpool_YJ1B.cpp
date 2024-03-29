#include "Whirlpool_YJ1B.hpp"
#include <ArduinoJson.h>
#include <stdex.hpp>
#include <map>

using namespace std;

static const std::map< bool, std::string_view > mapOnOff{ { true, "On" }, { false, "Off" } };
static const std::map< uint8_t, std::string_view > mapFan{ { 0, "Auto" }, { 1, "V1" }, { 2, "V2" }, { 3, "V3" } };
static const std::map< bool, std::string_view > mapMode{
	{ 0, "Sense_6th" }, { 1, "Cool" }, { 2, "Dry" }, { 3, "Fan" }, { 4, "Heat" }
};

WhirlpoolYJ1B::WhirlpoolYJ1B()
{
	setFan( Fan::Auto );
	setMode( Mode::Cool );
	setPower( false );
	setSwing( false );
	setJet( false );
	setLight( true );
	setSleep( false );
	setTemperature( 24 );
}

WhirlpoolYJ1B::WhirlpoolYJ1B( const WhirlpoolYJ1BData& data ) : m_data{ data } {}

WhirlpoolYJ1BData& WhirlpoolYJ1B::data()
{
	return m_data;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setMode( Mode mode )
{
	const auto data = tounderlying( mode );
	if( data > 4 )
		m_data.bytes.byte0.mode = 0;
	m_data.bytes.byte0.mode = data;

	return *this;
}

Mode WhirlpoolYJ1B::getMode() const
{
	return static_cast< Mode >( m_data.bytes.byte0.mode );
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setPower( bool power )
{
	m_data.bytes.byte0.power = power;
	return *this;
}

bool WhirlpoolYJ1B::getPower() const
{
	return m_data.bytes.byte0.power;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setFan( Fan fan )
{
	m_data.bytes.byte0.fan = tounderlying( fan );
	return *this;
}

Fan WhirlpoolYJ1B::getFan() const
{
	return static_cast< Fan >( m_data.bytes.byte0.fan );
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setSwing( bool swing )
{
	m_data.bytes.byte0.swing = swing;
	return *this;
}

bool WhirlpoolYJ1B::getSwing() const
{
	return m_data.bytes.byte0.swing;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setSleep( bool sleep )
{
	m_data.bytes.byte0.sleep = sleep;
	return *this;
}

bool WhirlpoolYJ1B::getSleep() const
{
	return m_data.bytes.byte0.sleep;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setJet( bool jet )
{
	m_data.bytes.byte6.jet = jet;
	return *this;
}

bool WhirlpoolYJ1B::getJet() const
{
	return m_data.bytes.byte6.jet;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setLight( bool light )
{
	m_data.bytes.byte6.light = light;
	return *this;
}

bool WhirlpoolYJ1B::getLight() const
{
	return m_data.bytes.byte6.light;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setTemperatureRaw( uint8_t temp )
{
	m_data.bytes.byte1.temperature = temp > 14 ? 14 : temp;
	return *this;
}

uint8_t WhirlpoolYJ1B::getTemperatureRaw() const
{
	return m_data.bytes.byte1.temperature;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setClockAMPM( AM_PM amPm )
{
	m_data.bytes.byte2.clock_AM_PM = tounderlying( amPm );
	return *this;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setClockAMPM( bool pm )
{
	m_data.bytes.byte2.clock_AM_PM = pm;
	return *this;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setClockSeconds( uint8_t seconds )
{
	m_data.bytes.byte1.clock_Seconds = ( seconds % 60 ) / 15;
	return *this;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setClockMinutes( uint8_t minutes )
{
	m_data.bytes.byte2.clock_Minutes = minutes % 60;
	return *this;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setClockHours( uint8_t hours )
{
	m_data.bytes.byte3.clock_Hours = hours % 12;
	if( hours >= 12 )
		setClockAMPM( true );
	return *this;
}

WhirlpoolYJ1B& WhirlpoolYJ1B::setTemperature( uint8_t temp )
{
	if( temp > 30 )
		m_data.bytes.byte1.temperature = 30 - 16;
	else if( temp < 16 )
		m_data.bytes.byte1.temperature = 0;
	else
		m_data.bytes.byte1.temperature = temp - 16;

	return *this;
}

uint8_t WhirlpoolYJ1B::getTemperature() const
{
	return m_data.bytes.byte1.temperature + 16;
}

string_view WhirlpoolYJ1B::getLightText() const
{
	return mapOnOff.at( getLight() );
}

string_view WhirlpoolYJ1B::getModeText() const
{
	return mapMode.at( tounderlying( getMode() ) );
}

string_view WhirlpoolYJ1B::getPowerText() const
{
	return mapOnOff.at( getPower() );
}

string_view WhirlpoolYJ1B::getFanText() const
{
	return mapFan.at( tounderlying( getFan() ) );
}

string_view WhirlpoolYJ1B::getJetText() const
{
	return mapOnOff.at( tounderlying( getJet() ) );
}

string_view WhirlpoolYJ1B::getSwingText() const
{
	return mapOnOff.at( tounderlying( getSwing() ) );
}

string_view WhirlpoolYJ1B::getSleepText() const
{
	return mapOnOff.at( getSleep() );
}

std::string WhirlpoolYJ1B::getJson() const
{
	StaticJsonDocument< 1024 > doc;
	doc[ "temperature" ] = getTemperature();
	doc[ "light" ]		 = getLight();
	doc[ "mode" ]		 = tounderlying( getMode() );
	doc[ "state" ]		 = getPower();
	doc[ "fan" ]		 = tounderlying( getFan() );
	doc[ "jet" ]		 = getJet();
	doc[ "swing" ]		 = getSwing();
	doc[ "sleep" ]		 = getSleep();

	char buffer[ 1024 ];
	serializeJson( doc, buffer );
	return string( buffer );
}