#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

template< int N >
class WhirlpoolYJ1B
{
public:
	static_assert( N > 0, "Size can't be zero" );
	struct Data
	{
		uint8_t data[ ( N + 7 ) / 8 ]{};
		const uint16_t dataSize{ N };
		const uint16_t storageSize{ ( N + 7 ) / 8 };
	};

public:
	WhirlpoolYJ1B() = default;
	WhirlpoolYJ1B( const Data& data ) : m_data( data ) {}
	virtual ~WhirlpoolYJ1B() = default;

	const Data& data() const
	{
		return m_data;
	}

private:
	Data m_data{};
};