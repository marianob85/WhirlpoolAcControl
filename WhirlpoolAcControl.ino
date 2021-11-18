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
#include "src/Whirlpool_YJ1B.hpp"

WhirlpoolYJ1B g_whirpool;

void setup()
{
	g_whirpool.setFan( Fan::Auto )
		.setJet( false )
		.setLight( true )
		.setMode( Mode::Cool )
		.setPower( false )
		.setSleep( false )
		.setSwing( false )
		.setTemperature( 22 );

	Serial.begin( 115200 );
	Serial.println( "Started" );
	IrSender.begin( true );
	IrSender.enableIROut( AC_KHZ );
}

/*
void send()
{
	// Header
	IrSender.mark( AC_HEADER_MARK );
	IrSender.space( AC_HEADER_SPACE );

	const uint32_t a = 0b00000010011100001101100100000100;
	const uint32_t b = 0b10100000001010101100000000000000;
	const uint32_t c = 0b0101;

	IrSender.sendPulseDistanceWidthData( AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, a, 32, false, false );
	IrSender.sendPulseDistanceWidthData( AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, b, 32, false, false );
	IrSender.sendPulseDistanceWidthData( AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, c, 4, false, true );
	delay( DELAY_AFTER_SEND );

	Serial.println( "Sended" );
}
*/

void send(){
	IrSender.mark( AC_HEADER_MARK );
	IrSender.space( AC_HEADER_SPACE );

	
	IrSender.sendPulseDistanceWidthData( AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, a, 32, false, false );

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
			whirpool.setMode( static_cast< Mode >( ( static_cast< uint8_t >( whirpool.getMode() ) + 1 )
												   % static_cast< uint8_t >( Mode::Heat ) ) );
			whirpool.printDebug();
		}
	}

	//  send();
}
