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
			uint8_t mode : 3;
			uint8_t power : 1;
			uint8_t fan : 2;
			uint8_t swing : 1;
			uint8_t sleep : 1;
		};
	};

	union Bit1
	{
		uint8_t raw;
		struct
		{
			uint8_t temperature : 4;   // Temperature 0: 16deg, 1: 17deg  etc..
			uint8_t clock_Seconds : 4; // 4 second interval - 60sec/15
		};
	};

	union Bit2
	{
		uint8_t raw;
		struct
		{
			uint8_t clock_Minutes : 6;
			uint8_t clock_AM_PM : 1;
			uint8_t unknown : 1;
		};
	};

	union Bit3
	{
		uint8_t raw;
		struct
		{
			uint8_t clock_Hours : 4;
			uint8_t timer_Enable_Minutes_LSB : 4;
		};
	};

	union Bit4
	{
		uint8_t raw;
		struct
		{
			uint8_t timer_Enable_Minutes_MSB : 2;
			uint8_t timer_Enable_AM_PM : 1;
			uint8_t timer_Enable_ON_OFF : 1;
			uint8_t timer_Enable_Hours : 4;
		};
	};

	union Bit5
	{
		uint8_t raw;
		struct
		{
			uint8_t timer_Disable_Minutes_LSB : 6;
			uint8_t timer_Disable_AM_PM : 1;
			uint8_t timer_Disable_ON_OFF : 1;
		};
	};

	union Bit6
	{
		uint8_t raw;
		struct
		{
			uint8_t timer_Disable_Hours : 4;
			uint8_t jet : 1;
			uint8_t light : 1;
			uint8_t unknown : 2;
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
	WhirlpoolYJ1B() = default;
	explicit WhirlpoolYJ1B( const WhirlpoolYJ1BData& data );
	virtual ~WhirlpoolYJ1B() = default;

	WhirlpoolYJ1BData& data();

	void setMode( Mode mode );
	void setPower( bool power );
	void setFan( Fan fan );
	void setSwitg( bool swing );
	void setSleep( bool sleep );

#ifdef WhirlpoolYJ1B_TRACE
	void WhirlpoolYJ1B::print();
#endif

private:
	WhirlpoolYJ1BData m_data{};
};