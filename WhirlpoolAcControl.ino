#define AC_KHZ 38
#define AC_UNIT 560						// 21.28 periods of 38 kHz
#define AC_HEADER_MARK ( 16 * AC_UNIT ) // 9000
#define AC_HEADER_SPACE ( 8 * AC_UNIT ) // 4500

#define AC_BIT_MARK AC_UNIT
#define AC_ONE_SPACE ( 3 * AC_UNIT ) // 1690
#define AC_ZERO_SPACE AC_UNIT

#define DELAY_AFTER_SEND 2000

#define SEND_PWM_BY_TIMER
#include "src/IRremote/IRremoteInt.h"
#include "src/IRremote/IRremote.h"

// PULSE_DISTANCE: HeaderMarkMicros=8950 HeaderSpaceMicros=4400 MarkMicros=600 OneSpaceMicros=1650 ZeroSpaceMicros=550
// 68 bits LSB first
void setup()
{
	Serial.begin( 115200 );
	IrSender.begin( true );
	IrSender.enableIROut( AC_KHZ );
	Serial.println( "Started" );
}

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

bool send_test = true;

void loop()
{
	send();
}
