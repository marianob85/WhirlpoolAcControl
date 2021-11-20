/*
 * IRSend.hpp
 *
 *  Contains common functions for sending
 *
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2009-2021 Ken Shirriff, Rafi Khan, Armin Joachimsmeyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */
#ifndef IR_SEND_HPP
#define IR_SEND_HPP

#include "IRremoteInt.h"
#if defined( IR_SEND_PIN )
#define sendPin IR_SEND_PIN
#include "digitalWriteFast.h"
#else
#define digitalWriteFast digitalWrite
#define pinModeFast pinMode
#endif

/** \addtogroup Sending Sending IR data for multiple protocols
 * @{
 */

// The sender instance
IRsend IrSender;

IRsend::IRsend()
{
	setLEDFeedback( 0, false );
}

#if !defined( IR_SEND_PIN ) && !defined( SEND_PWM_BY_TIMER )
IRsend::IRsend( uint8_t aSendPin )
{
	sendPin = aSendPin;
}

void IRsend::setSendPin( uint8_t aSendPin )
{
	sendPin = aSendPin;
}
#endif

#if defined( IR_SEND_PIN )
/**
 * Simple start with defaults for constant send pin
 */
void IRsend::begin()
{
	setLEDFeedback( USE_DEFAULT_FEEDBACK_LED_PIN, true );
}
#endif

/**
 * Initializes the send and feedback pin
 * @param aSendPin The Arduino pin number, where a IR sender diode is connected.
 * @param aLEDFeedbackPin if 0, then take board specific FEEDBACK_LED_ON() and FEEDBACK_LED_OFF() functions
 */
void IRsend::begin( uint8_t aSendPin, bool aEnableLEDFeedback, uint8_t aLEDFeedbackPin )
{
#if !defined( IR_SEND_PIN )
	sendPin = aSendPin;
#else
	( void )aSendPin;
#endif
	setLEDFeedback( aLEDFeedbackPin, aEnableLEDFeedback );
}

__attribute( ( error( "Error: You must use begin(<sendPin>, <EnableLEDFeedback>, <LEDFeedbackPin>) if IR_SEND_PIN and "
					  "SEND_PWM_BY_TIMER are not defined or USE_NO_SEND_PWM is defined. To suppress this error, enable "
					  "lto or activate the line #define SUPPRESS_ERROR_MESSAGE_FOR_BEGIN in IRremote.hpp." ) ) ) void
beginUsageError();

/**
 * @param aLEDFeedbackPin if 0, then take board specific FEEDBACK_LED_ON() and FEEDBACK_LED_OFF() functions
 */
void IRsend::begin( bool aEnableLEDFeedback, uint8_t aLEDFeedbackPin )
{
#if !defined( IR_SEND_PIN )

	// must exclude cores by MCUdude, MEGATINYCORE, NRF5, SAMD and ESP32 because they do not use the -flto flag for
	// compile
#if( !defined( SEND_PWM_BY_TIMER ) || defined( USE_NO_SEND_PWM ) ) && !defined( SUPPRESS_ERROR_MESSAGE_FOR_BEGIN ) \
	&& !( defined( NRF5 ) || defined( ARDUINO_ARCH_NRF52840 ) ) && !defined( ARDUINO_ARCH_SAMD )                   \
	&& !defined( ARDUINO_ARCH_RP2040 ) && !defined( ESP32 ) && !defined( ESP8266 ) && !defined( MEGATINYCORE )     \
	&& !defined( MINICORE ) && !defined( MIGHTYCORE ) && !defined( MEGACORE ) && !defined( MAJORCORE )             \
	&& !( defined( __STM32F1__ ) || defined( ARDUINO_ARCH_STM32F1 ) )                                              \
	&& !( defined( STM32F1xx ) || defined( ARDUINO_ARCH_STM32 ) )
	/*
	 * This error shows up, if this function is really used/called by the user program.
	 * This check works only if lto is enabled, otherwise it always pops up :-(.
	 * In this case activate the line #define SUPPRESS_ERROR_MESSAGE_FOR_BEGIN in IRremote.h to suppress this message.
	 * I know now way to check for lto flag here.
	 */
	beginUsageError();
#endif
#endif
	setLEDFeedback( aLEDFeedbackPin, aEnableLEDFeedback );
}

void IRsend::sendPulseDistanceWidthData( unsigned int aOneMarkMicros,
										 unsigned int aOneSpaceMicros,
										 unsigned int aZeroMarkMicros,
										 unsigned int aZeroSpaceMicros,
										 uint32_t aData,
										 uint8_t aNumberOfBits,
										 bool aMSBfirst,
										 bool aSendStopBit )
{

	if( aMSBfirst )
	{ // Send the MSB first.
		// send data from MSB to LSB until mask bit is shifted out
		for( uint32_t tMask = 1UL << ( aNumberOfBits - 1 ); tMask; tMask >>= 1 )
		{
			if( aData & tMask )
			{
				mark( aOneMarkMicros );
				space( aOneSpaceMicros );
			}
			else
			{
				mark( aZeroMarkMicros );
				space( aZeroSpaceMicros );
			}
		}
	}
	else
	{ // Send the Least Significant Bit (LSB) first / MSB last.
		for( uint_fast8_t bit = 0; bit < aNumberOfBits; bit++, aData >>= 1 )
			if( aData & 1 )
			{ // Send a 1
				mark( aOneMarkMicros );
				space( aOneSpaceMicros );
			}
			else
			{ // Send a 0
				mark( aZeroMarkMicros );
				space( aZeroSpaceMicros );
			}
	}
	if( aSendStopBit )
	{
		mark( aZeroMarkMicros ); // seems like this is used for stop bits
	}
}

void IRsend::sendPulseDistanceWidthRawData( unsigned int aOneMarkMicros,
											unsigned int aOneSpaceMicros,
											unsigned int aZeroMarkMicros,
											unsigned int aZeroSpaceMicros,
											uint8_t* data,
											uint8_t aNumberOfBits,
											bool sendStopBit )
{

	for( uint_fast8_t bit = 0; bit < aNumberOfBits; ++bit )
	{
		auto const markbit = data[ bit / 8 ] >> bit % 8;
		if( markbit & 1 )
		{ // Send a 1
			mark( aOneMarkMicros );
			space( aOneSpaceMicros );
		}
		else
		{ // Send a 0
			mark( aZeroMarkMicros );
			space( aZeroSpaceMicros );
		}
	}
	if( sendStopBit )
	{
		mark( aZeroMarkMicros ); // seems like this is used for stop bits
	}
}

void IRsend::mark( unsigned int aMarkMicros )
{

#if defined( SEND_PWM_BY_TIMER )
	setFeedbackLED( true );
ENABLE_SEND_PWM_BY_TIMER
; // Enable timer or ledcWrite() generated PWM output
customDelayMicroseconds( aMarkMicros );
IRLedOff();
setFeedbackLED( false );

#elif defined( USE_NO_SEND_PWM )
	setFeedbackLED( true );
#if defined( USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN ) && !defined( OUTPUT_OPEN_DRAIN )
	pinModeFast( sendPin, OUTPUT ); // active state for mimicking open drain
#else
	digitalWriteFast( sendPin, LOW );  // Set output to active low.
#endif

	customDelayMicroseconds( aMarkMicros );
	IRLedOff();
	setFeedbackLED( false );

#else
	unsigned long startMicros	   = micros();
	unsigned long nextPeriodEnding = startMicros;
	unsigned long tMicros;
	bool FeedbackLedIsActive = false;

	do
	{
		//        digitalToggleFast(_IR_TIMING_TEST_PIN);
		// Output the PWM pulse
		noInterrupts();							 // do not let interrupts extend the short on period
#if defined( USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN )
#if defined( OUTPUT_OPEN_DRAIN )
		digitalWriteFast( sendPin, LOW );		 // active state for open drain
#else
		pinModeFast( sendPin, OUTPUT ); // active state for mimicking open drain
#endif
#else
#if defined( IR_SEND_PIN )
		digitalWriteFast( IR_SEND_PIN, HIGH ); // 3.5 us from FeedbackLed on to pin setting. 5.7 us from call of mark()
											   // to pin setting incl. setting of feedback pin.
#else
		digitalWrite( sendPin, HIGH ); // 4.3 us from do{ to pin setting
#endif
#endif
		delayMicroseconds( periodOnTimeMicros ); // this is normally implemented by a blocking wait

		// Output the PWM pause
#if defined( USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN ) && !defined( OUTPUT_OPEN_DRAIN )
#if defined( OUTPUT_OPEN_DRAIN )
		digitalWriteFast( sendPin, HIGH ); // Set output to inactive high.
#else
		pinModeFast( sendPin, INPUT );	// inactive state to mimic open drain
#endif

#else
		digitalWriteFast( sendPin, LOW );
#endif
		interrupts(); // Enable interrupts -to keep micros correct- for the longer off period 3.4 us until receive ISR
					  // is active (for 7 us + pop's)

		/*
		 * Delayed call of setFeedbackLED() to get better timing
		 */
		if( !FeedbackLedIsActive )
		{
			FeedbackLedIsActive = true;
			setFeedbackLED( true );
		}

		/*
		 * Pause timing
		 */
		nextPeriodEnding += periodTimeMicros;
		do
		{
			tMicros = micros(); // we have only 4 us resolution for AVR @16MHz
			// check for aMarkMicros to be gone
			unsigned int tDeltaMicros = tMicros - startMicros;
#if defined( __AVR__ )
			//            tDeltaMicros += (160 / CLOCKS_PER_MICRO); // adding this once increases program size !
			if( tDeltaMicros >= aMarkMicros - ( 30 + ( 112 / CLOCKS_PER_MICRO ) ) )
			{ // 30 to be constant. Using periodTimeMicros increases program size too much.
				// reset feedback led in the last pause before end
				setFeedbackLED( false );
			}
			if( tDeltaMicros >= aMarkMicros - ( 112 / CLOCKS_PER_MICRO ) )
			{ // To compensate for call duration - 112 is an empirical value
#else
			if( tDeltaMicros >= aMarkMicros )
			{
#endif
				return;
			}
			//            digitalToggleFast(_IR_TIMING_TEST_PIN); // 3.0 us per call @16MHz
		} while( tMicros < nextPeriodEnding ); // 3.4 us @16MHz
	} while( true );
#endif
}

/**
 * Just switch the IR sending LED off to send an IR space
 * A space is "no output", so the PWM output is disabled.
 * This function may affect the state of feedback LED.
 */
void IRsend::IRLedOff()
{
#if defined( SEND_PWM_BY_TIMER )
DISABLE_SEND_PWM_BY_TIMER
; // Disable PWM output
#elif defined( USE_NO_SEND_PWM )
#if defined( USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN ) && !defined( OUTPUT_OPEN_DRAIN )
	digitalWriteFast( sendPin, LOW ); // prepare for all next active states.
	pinModeFast( sendPin, INPUT );	  // inactive state for open drain
#else
	digitalWriteFast( sendPin, HIGH ); // Set output to inactive high.
#endif
#else
#if defined( USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN )
#if defined( OUTPUT_OPEN_DRAIN )
	digitalWriteFast( sendPin, HIGH ); // Set output to inactive high.
#else
	pinModeFast( sendPin, INPUT );		// inactive state to mimic open drain
#endif
#else
	digitalWriteFast( sendPin, LOW );
#endif
#endif

setFeedbackLED( false );
}

/**
 * Sends an IR space for the specified number of microseconds.
 * A space is "no output", so just wait.
 */
void IRsend::space( unsigned int aSpaceMicros )
{
	customDelayMicroseconds( aSpaceMicros );
}

/**
 * Custom delay function that circumvents Arduino's delayMicroseconds 16 bit limit
 * and is (mostly) not extended by the duration of interrupt codes like the millis() interrupt
 */
void IRsend::customDelayMicroseconds( unsigned long aMicroseconds )
{
#if defined( __AVR__ )
	unsigned long start = micros() - ( 64 / clockCyclesPerMicrosecond() ); // - (64 / clockCyclesPerMicrosecond()) for
																		   // reduced resolution and additional overhead
#else
	unsigned long start = micros();
#endif
	// overflow invariant comparison :-)
	while( micros() - start < aMicroseconds )
	{
	}
}

/**
 * Enables IR output. The kHz value controls the modulation frequency in kilohertz.
 * The IR output will be on pin 3 (OC2B).
 * This routine is designed for 36-40 kHz and for software generation gives 26 us for 38.46 kHz, 27 us for 37.04 kHz and
 * 25 us for 40 kHz. If you use it for other values, it's up to you to make sure it gives reasonable results.  (Watch
 * out for overflow / underflow / rounding.) TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the
 * frequency and OCR2B controlling the duty cycle. There is no prescaling, so the output frequency is 16 MHz / (2 *
 * OCR2A) To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin. A few
 * hours staring at the ATmega documentation and this will all make sense. See my Secrets of Arduino PWM at
 * http://www.righto.com/2009/07/secrets-of-arduino-pwm.html for details.
 */
void IRsend::enableIROut( uint8_t aFrequencyKHz )
{
#if defined( SEND_PWM_BY_TIMER )
#if defined( SEND_PWM_BY_TIMER )
	TIMER_DISABLE_RECEIVE_INTR;
#endif
	timerConfigForSend( aFrequencyKHz );

#elif defined( USE_NO_SEND_PWM )
	( void )aFrequencyKHz;

#else
	periodTimeMicros = ( 1000U + ( aFrequencyKHz / 2 ) )
		/ aFrequencyKHz; // rounded value -> 26 for 38.46 kHz, 27 for 37.04 kHz, 25 for 40 kHz.
#if defined( IR_SEND_PIN )
	periodOnTimeMicros = ( ( ( periodTimeMicros * IR_SEND_DUTY_CYCLE ) + 50 )
						   / 100U ); // +50 for rounding -> 830/100 for 30% and 16 MHz
#else
	periodOnTimeMicros = ( ( ( periodTimeMicros * IR_SEND_DUTY_CYCLE ) + 50 - ( PULSE_CORRECTION_NANOS / 10 ) )
						   / 100U ); // +50 for rounding -> 530/100 for 30% and 16 MHz
#endif
#endif

#if defined( USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN )
#if defined( OUTPUT_OPEN_DRAIN )
	pinMode( sendPin, OUTPUT_OPEN_DRAIN ); // the only place where this mode is set for sendPin
#endif									   // the mode INPUT for mimicking open drain is set at IRLedOff()

#else
	pinModeFast( sendPin, OUTPUT );
#endif
	IRLedOff(); // When not sending, we want it low/inactive
}

/** @}*/
#endif // IR_SEND_HPP
#pragma once
