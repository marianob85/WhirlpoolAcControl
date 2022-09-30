#pragma once
#include <string>
#include <dhtnew.h>

class DHTSensor
{
public:
	DHTSensor( uint8_t pin );
	void loop( unsigned readRate );

	std::string getJson() const;

private:
	DHTNEW m_sensor;

	std::optional< float > m_humidity{};
	std::optional< float > m_temperature{};
	std::optional< int > m_lastError{ DHTLIB_WAITING_FOR_READ };
};