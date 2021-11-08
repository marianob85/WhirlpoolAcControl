#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

enum class Fan : uint8_t
{
	Auto,
	V1,
	V2,
	V3
};

enum class Mode : uint8_t
{
	Sense_6th,
	Cool,
	Dry,
	Fan,
	Heat
};

enum class AM_PM : uint8_t
{
	AM,
	PM
};

union WhirlpoolYJ1BData
{
	union Bit0
	{
		uint8_t raw;
		struct
		{
			Mode Mode : 3;
			uint8_t Power : 1;
			Fan Fan : 2;
			uint8_t Swing : 1;
			uint8_t Sleep : 1;
		};
	};

	union Bit1
	{
		uint8_t raw;
		struct
		{
			uint8_t Temperature : 4;   // Temperature 0: 16deg, 1: 17deg  etc..
			uint8_t Clock_Seconds : 4; // 4 second interval - 60sec/15
		};
	};

	union Bit2
	{
		uint8_t raw;
		struct
		{
			uint8_t Clock_Minutes : 6;
			AM_PM Clock_AM_PM : 1;
			uint8_t Unknown : 1;
		};
	};

	union Bit3
	{
		uint8_t raw;
		struct
		{
			uint8_t Clock_Hours : 4;
			uint8_t Timer_Enable_Minutes_LSB : 4;
		};
	};

	union Bit4
	{
		uint8_t raw;
		struct
		{
			uint8_t Timer_Enable_Minutes_MSB : 2;
			AM_PM Timer_Enable_AM_PM : 1;
			uint8_t Timer_Enable_ON_OFF : 1;
			uint8_t Timer_Enable_Hours : 4;
		};
	};

	union Bit5
	{
		uint8_t raw;
		struct
		{
			uint8_t Timer_Disable_Minutes_LSB : 6;
			AM_PM Timer_Disable_AM_PM : 1;
			uint8_t Timer_Disable_ON_OFF : 1;
		};
	};

	union Bit6
	{
		uint8_t raw;
		struct
		{
			uint8_t Timer_Disable_Hours : 4;
			uint8_t Jet : 1;
			uint8_t Light : 1;
			uint8_t Unknown : 2;
		};
	};

	// Bit 7,8,9 -> Unknown

	uint8_t raw[ 9 ];
	struct
	{
		Bit0 bit0;
		Bit1 bit1;
		Bit2 bit2;
		Bit3 bit3;
		Bit4 bit4;
		Bit5 bit5;
		Bit6 bit6;
	};
};

class WhirlpoolYJ1B
{
public:
public:
	WhirlpoolYJ1B() = default;
	explicit WhirlpoolYJ1B( const WhirlpoolYJ1BData& data );
	virtual ~WhirlpoolYJ1B() = default;

	WhirlpoolYJ1BData& data();

#ifdef WhirlpoolYJ1B_TRACE
	void print();
#endif

private:
	WhirlpoolYJ1BData m_data{};
};