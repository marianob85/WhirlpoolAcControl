/**
 * @file IRremote.hpp
 *
 * @brief Public API to the library.
 *
 * @code
 * !!! All the macro values defined here can be overwritten with values,    !!!
 * !!! the user defines in its source code BEFORE the #include <IRremote.hpp> !!!
 * @endcode
 *
 * This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2015-2021 Ken Shirriff http://www.righto.com, Rafi Khan, Armin Joachimsmeyer
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
 *
 * For Ken Shiriffs original blog entry, see http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html
 * Initially influenced by:
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * and http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 */

#ifndef IRremote_hpp
#define IRremote_hpp

#define VERSION_IRREMOTE "3.5.0"
#define VERSION_IRREMOTE_MAJOR 3
#define VERSION_IRREMOTE_MINOR 5

// activate it for all cores that does not use the -flto flag, if you get false error messages regarding begin() during
// compilation.
//#define SUPPRESS_ERROR_MESSAGE_FOR_BEGIN

/*
 * If activated, BOSEWAVE, MAGIQUEST,WHYNTER and LEGO_PF are excluded in decoding and in sending with IrSender.write
 */
//#define EXCLUDE_EXOTIC_PROTOCOLS
/****************************************************
 *                     PROTOCOLS
 ****************************************************/
/*
 * Supported IR protocols
 * Each protocol you include costs memory and, during decode, costs time
 * Copy the lines with the protocols you need in your program before the  #include <IRremote.hpp> line
 * See also SimpleReceiver example
 */
//#define DECODE_MAGIQUEST // This must be enabled explicitly, since it modifies the RAW_BUFFER_LENGTH from 100 to 112

#if !defined( RAW_BUFFER_LENGTH )
#if defined( DECODE_MAGIQUEST )
#define RAW_BUFFER_LENGTH 112 // MagiQuest requires 112 bytes.
#else
#define RAW_BUFFER_LENGTH \
	100 ///< Maximum length of raw duration buffer. Must be even. 100 supports up to 48 bit codings inclusive 1 start
		///< and 1 stop bit.
//#define RAW_BUFFER_LENGTH  750  // 750 is the value for air condition remotes.
#endif
#endif
#if RAW_BUFFER_LENGTH % 2 == 1
#error RAW_BUFFER_LENGTH must be even, since the array consists of space / mark pairs.
#endif

/**
 * MARK_EXCESS_MICROS is subtracted from all marks and added to all spaces before decoding,
 * to compensate for the signal forming of different IR receiver modules
 * For Vishay TSOP*, marks tend to be too long and spaces tend to be too short.
 * If you set MARK_EXCESS_MICROS to approx. 50us then the TSOP4838 works best.
 * At 100us it also worked, but not as well.
 * Set MARK_EXCESS to 100us and the VS1838 doesn't work at all.
 *
 * The right value is critical for IR codes using short pulses like Denon / Sharp / Lego
 *
 *  Observed values:
 *  Delta of each signal type is around 50 up to 100 and at low signals up to 200. TSOP is better, especially at low IR
 * signal level. VS1838      Mark Excess -50 to +50 us TSOP31238   Mark Excess 0 to +50
 */
#if !defined( MARK_EXCESS_MICROS )
// To change this value, you simply can add a line #define "MARK_EXCESS_MICROS <My_new_value>" in your ino file before
// the line "#include <IRremote.hpp>"
#define MARK_EXCESS_MICROS 20
#endif

/**
 * Minimum gap between IR transmissions, to detect the end of a protocol.
 * Must be greater than any space of a protocol e.g. the NEC header space of 4500 us.
 * Must be smaller than any gap between a command and a repeat; e.g. the retransmission gap for Sony is around 24 ms.
 * Keep in mind, that this is the delay between the end of the received command and the start of decoding.
 */
#if !defined( RECORD_GAP_MICROS )
// To change this value, you simply can add a line #define "RECORD_GAP_MICROS <My_new_value>" in your ino file before
// the line "#include <IRremote.hpp>"
#define RECORD_GAP_MICROS 5000 // FREDRICH28AC / LG2 header space is 9700, NEC header space is 4500
#endif
/**
 * Threshold for warnings at printIRResult*() to report about changing the RECORD_GAP_MICROS value to a higher value.
 */
#if !defined( RECORD_GAP_MICROS_WARNING_THRESHOLD )
// To change this value, you simply can add a line #define "RECORD_GAP_MICROS_WARNING_THRESHOLD <My_new_value>" in your
// ino file before the line "#include <IRremote.hpp>"
#define RECORD_GAP_MICROS_WARNING_THRESHOLD 20000
#endif

/** Minimum gap between IR transmissions, in MICROS_PER_TICK */
#define RECORD_GAP_TICKS ( RECORD_GAP_MICROS / MICROS_PER_TICK ) // 221 for 1100

/*
 * Activate this line if your receiver has an external output driver transistor / "inverted" output
 */
//#define IR_INPUT_IS_ACTIVE_HIGH
#ifdef IR_INPUT_IS_ACTIVE_HIGH
// IR detector output is active high
#define INPUT_MARK 1 ///< Sensor output for a mark ("flash")
#else
// IR detector output is active low
#define INPUT_MARK 0 ///< Sensor output for a mark ("flash")
#endif
/****************************************************
 *                     SENDING
 ****************************************************/
/**
 * Define to disable carrier PWM generation in software and use (restricted) hardware PWM.
 */
#if defined( ESP32 )
#define SEND_PWM_BY_TIMER // the best and default method for ESP32
#else
//#define SEND_PWM_BY_TIMER
#endif

/**
 * Define to use no carrier PWM, just simulate an active low receiver signal.
 */
//#define USE_NO_SEND_PWM
#if defined( SEND_PWM_BY_TIMER ) && defined( USE_NO_SEND_PWM )
#undef SEND_PWM_BY_TIMER // USE_NO_SEND_PWM overrides SEND_PWM_BY_TIMER
#warning "SEND_PWM_BY_TIMER and USE_NO_SEND_PWM are both defined -> undefine SEND_PWM_BY_TIMER now!"
#endif

/**
 * Define to use or simulate open drain output mode at send pin.
 * Attention, active state of open drain is LOW, so connect the send LED between positive supply and send pin!
 */
//#define USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN
/**
 * This amount is subtracted from the on-time of the pulses generated for software PWM generation.
 * It should be the time used for digitalWrite(sendPin, LOW) and the call to delayMicros()
 * Measured value for Nano @16MHz is around 3000, for Bluepill @72MHz is around 700, for Zero 3600
 */
#if !defined( PULSE_CORRECTION_NANOS )
#if defined( F_CPU )
// To change this value, you simply can add a line #define "PULSE_CORRECTION_NANOS <My_new_value>" in your ino file
// before the line "#include <IRremote.hpp>"
#define PULSE_CORRECTION_NANOS ( 48000L / ( F_CPU / MICROS_IN_ONE_SECOND ) ) // 3000 @16MHz, 666 @72MHz
#else
#define PULSE_CORRECTION_NANOS 600
#endif
#endif

#include "IRremoteInt.h"
#include "IRTimer.hpp"
#include "IRFeedbackLED.hpp"
/*
 * Include the sources here to enable compilation with macro values set by user program.
 */
#include "IRSend.hpp"

#define RAWBUF 101 // Maximum length of raw duration buffer
#define REPEAT 0xFFFFFFFF
#define USECPERTICK MICROS_PER_TICK
#define MARK_EXCESS MARK_EXCESS_MICROS

#endif // IRremote_hpp

#pragma once
