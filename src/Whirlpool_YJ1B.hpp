#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

template< int N >
class WhirlpoolYJ1B
{
	template< int T >
	class Data
	{
		uint16_t m_size{ T };
		uint8_t m_data[ ( T + 1 ) / 8 ]{};
	};

public:
	virtual ~WhirlpoolYJ1B() = default;
	
protected:
	const Data& data() const
	{
		return m_data;
	}

private:
	Data< N > m_data{};
};