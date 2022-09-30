#include <ArduinoJson.h>
#include "DHTSensor.hpp"

using namespace std;

DHTSensor::DHTSensor( uint8_t pin ) : m_sensor( pin ) {}

void DHTSensor::loop( unsigned readRate )
{
	static auto lastTime = millis();
	if( millis() - lastTime > readRate )
	{
		lastTime = millis();

		m_humidity.reset();
		m_temperature.reset();
		m_lastError.reset();

		if( const auto ret = m_sensor.read(); ret == DHTLIB_OK )
		{
			m_humidity	  = m_sensor.getHumidity();
			m_temperature = m_sensor.getTemperature();
		}
		else
			m_lastError = ret;
	}
}

std::string DHTSensor::getJson() const
{
	StaticJsonDocument< 1024 > doc;
	doc[ "valid" ] = !m_lastError.has_value();
	if( !m_lastError )
	{
		doc[ "humidity" ]	 = *m_humidity;
		doc[ "temperature" ] = *m_temperature;
	}
	else
		doc[ "error" ] = *m_lastError;

	char buffer[ 1024 ];
	serializeJson( doc, buffer );
	return string( buffer );
}