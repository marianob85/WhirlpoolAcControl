#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

enum class Fan : uint8_t
{
	Auto = 0,
	V1	 = 1,
	V2	 = 2,
	V3	 = 3
};

enum class Mode : uint8_t
{
	Sense_6th = 0,
	Cool	  = 1,
	Dry		  = 2,
	Fan		  = 3,
	Heat	  = 4
};

enum class AM_PM : bool
{
	AM = false,
	PM = true
};

union WhirlpoolYJ1BData
{
	union Bit0
	{
		uint8_t raw;
		struct SBit0
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
		struct SBit1
		{
			uint8_t temperature : 4;   // Temperature 0: 16deg, 1: 17deg  etc..
			uint8_t clock_Seconds : 4; // 4 second interval - 60sec/15
		};
	};

	union Bit2
	{
		uint8_t raw;
		struct SBit2
		{
			uint8_t clock_Minutes : 6;
			uint8_t clock_AM_PM : 1;
			uint8_t unknown : 1;
		};
	};

	union Bit3
	{
		uint8_t raw;
		struct SBit3
		{
			uint8_t clock_Hours : 4;
			uint8_t timer_Enable_Minutes_LSB : 4;
		};
	};

	union Bit4
	{
		uint8_t raw;
		struct SBit4
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
		struct SBit5
		{
			uint8_t timer_Disable_Minutes_LSB : 6;
			uint8_t timer_Disable_AM_PM : 1;
			uint8_t timer_Disable_ON_OFF : 1;
		};
	};

	union Bit6
	{
		uint8_t raw;
		struct SBit6
		{
			uint8_t timer_Disable_Hours : 4;
			uint8_t jet : 1;
			uint8_t light : 1;
			uint8_t unknown : 2;
		};
	};

	// Bit 7,8 -> Unknown

	uint8_t raw[ 9 ];
	struct Bits
	{
		Bit0::SBit0 bit0;
		Bit1::SBit1 bit1;
		Bit2::SBit2 bit2;
		Bit3::SBit3 bit3;
		Bit4::SBit4 bit4;
		Bit5::SBit5 bit5;
		Bit6::SBit6 bit6;
	} bits;
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
	void setJet( bool jet );
	void setLight( bool light );
	void setTemperature( uint8_t temp );

#ifdef TRACE
	void WhirlpoolYJ1B::print();
#endif

private:
	WhirlpoolYJ1BData m_data{};
};