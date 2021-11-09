#define IR_RECEIVE_PIN 7 // To be compatible with interrupt example, pin 2 is chosen here.
#define IR_SEND_PIN 2

#define AC_KHZ 38
#define AC_UNIT 560						// 21.28 periods of 38 kHz
#define AC_HEADER_MARK ( 16 * AC_UNIT ) // 9000
#define AC_HEADER_SPACE ( 8 * AC_UNIT ) // 4500

#define AC_BIT_MARK AC_UNIT
#define AC_ONE_SPACE ( 3 * AC_UNIT ) // 1690
#define AC_ZERO_SPACE AC_UNIT

#include "IRremoteInt.h"
#include <IRremote.h>

// PULSE_DISTANCE: HeaderMarkMicros=8950 HeaderSpaceMicros=4400 MarkMicros=600 OneSpaceMicros=1650 ZeroSpaceMicros=550
// 68 bits LSB first
void setup()
{
	Serial.begin( 115200 );
	IrReceiver.begin( IR_RECEIVE_PIN, false );
	//IrReceiver.begin( IR_RECEIVE_PIN, false );
	// IrSender.begin( IR_SEND_PIN, true );
	// IrSender.enableIROut( AC_KHZ );
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

	IrSender.sendPulseDistanceWidthData(
		AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, a, 32, PROTOCOL_IS_LSB_FIRST, false );
	IrSender.sendPulseDistanceWidthData(
		AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, b, 32, PROTOCOL_IS_LSB_FIRST, false );
	IrSender.sendPulseDistanceWidthData(
		AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, c, 4, PROTOCOL_IS_LSB_FIRST, false );
	delay( DELAY_AFTER_SEND );

	Serial.println( "Sended" );
}
*/

void loop()
{
	// send();
	// return;
	if( IrReceiver.decode() )
	{
		Serial.println( "" );
		// Serial.println( "Received" );

		// IrReceiver.printIRResultRawFormatted( &Serial );

		/*
		 * !!!Important!!! Enable receiving of the next value,
		 * since receiving has stopped after the end of the current received data packet.
		 */
		IrReceiver.resume();
	}
}
