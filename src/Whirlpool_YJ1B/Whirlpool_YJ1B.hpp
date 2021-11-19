#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <iostream>
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
	union Byte0
	{
		uint8_t raw;
		struct SByte0
		{
			uint8_t mode : 3;
			uint8_t power : 1;
			uint8_t fan : 2;
			uint8_t swing : 1;
			uint8_t sleep : 1;
		};
	};

	union Byte1
	{
		uint8_t raw;
		struct SByte1
		{
			uint8_t temperature : 4;   // Temperature 0: 16deg, 1: 17deg  etc..
			uint8_t clock_Seconds : 4; // 4 second interval - 60sec/15
		};
	};

	union Byte2
	{
		uint8_t raw;
		struct SByte2
		{
			uint8_t clock_Minutes : 6;
			uint8_t clock_AM_PM : 1;
			uint8_t unknown : 1;
		};
	};

	union Byte3
	{
		uint8_t raw;
		struct SByte3
		{
			uint8_t clock_Hours : 4;
			uint8_t timer_Enable_Minutes_LSB : 4;
		};
	};

	union Byte4
	{
		uint8_t raw;
		struct SByte4
		{
			uint8_t timer_Enable_Minutes_MSB : 2;
			uint8_t timer_Enable_AM_PM : 1;
			uint8_t timer_Enable_ON_OFF : 1;
			uint8_t timer_Enable_Hours : 4;
		};
	};

	union Byte5
	{
		uint8_t raw;
		struct SByte5
		{
			uint8_t timer_Disable_Minutes_LSB : 6;
			uint8_t timer_Disable_AM_PM : 1;
			uint8_t timer_Disable_ON_OFF : 1;
		};
	};

	union Byte6
	{
		uint8_t raw;
		struct SByte6
		{
			uint8_t timer_Disable_Hours : 4;
			uint8_t jet : 1;
			uint8_t light : 1;
			uint8_t unknown : 2;
		};
	};

	// Byte 7,8 -> Unknown

	uint8_t raw[ 9 ];
	struct Bytes
	{
		Byte0::SByte0 bit0;
		Byte1::SByte1 bit1;
		Byte2::SByte2 bit2;
		Byte3::SByte3 bit3;
		Byte4::SByte4 bit4;
		Byte5::SByte5 bit5;
		Byte6::SByte6 bit6;
	} bytes;
};

class WhirlpoolYJ1B
{
public:
	WhirlpoolYJ1B() = default;
	explicit WhirlpoolYJ1B( const WhirlpoolYJ1BData& data );
	virtual ~WhirlpoolYJ1B() = default;

	WhirlpoolYJ1BData& data();

	WhirlpoolYJ1B& setMode( Mode mode );
	Mode getMode() const;
	WhirlpoolYJ1B& setPower( bool power );
	bool getPower() const;
	WhirlpoolYJ1B& setFan( Fan fan );
	Fan getFan() const;
	WhirlpoolYJ1B& setSwing( bool swing );
	bool getSwing() const;
	WhirlpoolYJ1B& setSleep( bool sleep );
	bool getSleep() const;
	WhirlpoolYJ1B& setJet( bool jet );
	bool getJet() const;
	WhirlpoolYJ1B& setLight( bool light );
	bool getLight() const;
	WhirlpoolYJ1B& setTemperatureRaw( uint8_t temp );
	uint8_t getTemperatureRaw() const;

	WhirlpoolYJ1B& setTemperature( uint8_t temp );
	uint8_t getTemperature() const;

#ifdef TRACE
	void printDebug()
	{
		print( "Raw: " );
		for( auto data : m_data.raw )
		{
			printBin( data );
			print( " " );
		}

		println( "" );

		auto printBool = [ & ]( const char* text, const uint8_t data )
		{
			const char* on	= "ON";
			const char* off = "OFF";
			print( text );
			println( data ? on : off );
		};

		print( "Mode: " );
		switch( Mode( m_data.bytes.bit0.mode ) )
		{
		case Mode::Sense_6th:
			println( "6th Sense" );
			break;
		case Mode::Cool:
			println( "Cool" );
			break;
		case Mode::Dry:
			println( "Dry" );
			break;
		case Mode::Fan:
			println( "Fan" );
			break;
		case Mode::Heat:
			println( "Heat" );
			break;
		default:
			println( "Unknown" );
			break;
		};

		printBool( "Power: ", m_data.bytes.bit0.power );

		print( "Fan:" );
		switch( Fan( m_data.bytes.bit0.fan ) )
		{
		case Fan::Auto:
			println( "Auto" );
			break;
		case Fan::V1:
			println( "V1" );
			break;
		case Fan::V2:
			println( "V2" );
			break;
		case Fan::V3:
			println( "V3" );
			break;
		default:
			println( "Unknown" );
			break;
		};

		printBool( "Swing: ", m_data.bytes.bit0.swing );
		printBool( "Sleep: ", m_data.bytes.bit0.sleep );
		printBool( "Light: ", m_data.bytes.bit6.light );
		printBool( "Jet: ", m_data.bytes.bit6.jet );

		print( "Temperature: " );
		println( m_data.bytes.bit1.temperature + 16 );
	}

#ifdef ARDUINO
	template< typename T >
	void println( const T text )
	{
		Serial.println( text );
	}
	template< typename T >
	void print( const T text )
	{
		Serial.print( text );
	}
	void printBin( uint8_t val )
	{
		Serial.print( val, BIN );
	}
#else
	template< typename T >
	void println( const T text )
	{
		std::cout << text << std::endl;
	}
	template< typename T >
	void print( const T text )
	{
		std::cout << text;
	}
	void printBin( uint8_t val ) {}
#endif
#endif

private:
	WhirlpoolYJ1BData m_data{};
};