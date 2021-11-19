#define AC_KHZ 38
#define AC_UNIT 560						// 21.28 periods of 38 kHz
#define AC_HEADER_MARK ( 16 * AC_UNIT ) // 9000
#define AC_HEADER_SPACE ( 8 * AC_UNIT ) // 4500

#define AC_BIT_MARK AC_UNIT
#define AC_ONE_SPACE ( 3 * AC_UNIT ) // 1690
#define AC_ZERO_SPACE AC_UNIT

#define DELAY_AFTER_SEND 2000

#define SEND_PWM_BY_TIMER
#define TRACE

#include "src/IRremote/IRremote.hpp"
#include "src/Whirlpool_YJ1B/Whirlpool_YJ1B.hpp"
#include "libs/Adafruit_BusIO/Adafruit_I2CDevice.h"
#include "src/RTClib/src/RTClib.h"

WhirlpoolYJ1B g_whirpool;
RTC_DS1307 rtc;

/*
#include <Wire.h>
void setup()
{
	Wire.begin();		  // Wire communication begin
	Serial.begin( 9600 ); // The baudrate of Serial monitor is set in 9600
	while( !Serial )
		; // Waiting for Serial Monitor
	Serial.println( "\nI2C Scanner" );
}

void loop()
{
	byte error, address; // variable for error and I2C address
	int nDevices;

	Serial.println( "Scanning..." );

	nDevices = 0;
	for( address = 1; address < 127; address++ )
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission( address );
		error = Wire.endTransmission();

		if( error == 0 )
		{
			Serial.print( "I2C device found at address 0x" );
			if( address < 16 )
				Serial.print( "0" );
			Serial.print( address, HEX );
			Serial.println( "  !" );
			nDevices++;
		}
		else if( error == 4 )
		{
			Serial.print( "Unknown error at address 0x" );
			if( address < 16 )
				Serial.print( "0" );
			Serial.println( address, HEX );
		}
	}
	if( nDevices == 0 )
		Serial.println( "No I2C devices found\n" );
	else
		Serial.println( "done\n" );

	delay( 5000 ); // wait 5 seconds for the next I2C scan
}
*/

// 0x68

void setup()
{

	Serial.begin( 115200 );
	Serial.println( "Started" );

	IrSender.begin( true );
	IrSender.enableIROut( AC_KHZ );

	g_whirpool.setFan( Fan::Auto )
		.setJet( false )
		.setLight( true )
		.setMode( Mode::Cool )
		.setPower( false )
		.setSleep( false )
		.setSwing( false )
		.setTemperature( 22 );

	if( !rtc.begin() )
	{
		Serial.println( "Couldn't find RTC" );
		Serial.flush();
		while( 1 )
			delay( 10 );
	}

	if( !rtc.isrunning() )
	{
		Serial.println( "RTC is NOT running, let's set the time!" );
		// When time needs to be set on a new device, or after a power loss, the
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust( DateTime( F( __DATE__ ), F( __TIME__ ) ) );
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}
}

void send()
{
	IrSender.mark( AC_HEADER_MARK );
	IrSender.space( AC_HEADER_SPACE );

	IrSender.sendPulseDistanceWidthRawData(
		AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, g_whirpool.data().raw, 68 );

	delay( DELAY_AFTER_SEND );
}

void loop()
{
	if( Serial.available() > 0 )
	{
		const auto incomingByte = Serial.read();
		switch( incomingByte )
		{
		case '1':
			g_whirpool.setMode( static_cast< Mode >( ( static_cast< uint8_t >( g_whirpool.getMode() ) + 1 )
													 % static_cast< uint8_t >( Mode::Heat ) ) );
			g_whirpool.printDebug();
			send();
		}
	}
}