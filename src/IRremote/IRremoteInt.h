/**
 * @file IRremoteInt.h
 * @brief Contains all declarations required for the interface to IRremote.
 * Could not be named IRremote.h, since this has another semantic for old example code found in the wild,
 * because it must include all *.hpp files.
 *
 * This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
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
 */
#ifndef IRremoteInt_h
#define IRremoteInt_h

#include <Arduino.h>

/*
 * !!! 3 macros which are evaluated in this file and must be consistent with the definitions in the ino file if they are not already defined !!!
 * RAW_BUFFER_LENGTH and IR_SEND_PIN and SEND_PWM_BY_TIMER
 * Modify or keep the 3 values below, if you use #include IRremoteInt.h in a file not containing main().
 * !!! RAW_BUFFER_LENGTH must have the same value for ALL compilation units !!!
 * Otherwise you will see warnings like: "warning: type 'struct irparams_struct' violates the C++ One Definition Rule"
 */
#if !defined(RAW_BUFFER_LENGTH)
//#define RAW_BUFFER_LENGTH  100 // 100 is default
//#define RAW_BUFFER_LENGTH  112 //  MagiQuest requires 112 bytes. enable this if DECODE_MAGIQUEST is enabled
#endif
#if !defined(IR_SEND_PIN)
//#define IR_SEND_PIN            // here it is only interesting if it is defined, the value does not matter here
#endif
#if !defined(SEND_PWM_BY_TIMER)
//#define SEND_PWM_BY_TIMER      // here it is only interesting if it is defined, there is no value anyway
#endif
#if !defined(RAW_BUFFER_LENGTH)
#error Seems you use #include IRremoteInt.h in a file not containing main(). Please define RAW_BUFFER_LENGTH with the same value as in the main program and check if the macros IR_SEND_PIN and SEND_PWM_BY_TIMER are defined in the main program.
#endif

#define MARK   1
#define SPACE  0

#define MILLIS_IN_ONE_SECOND 1000L
#define MICROS_IN_ONE_SECOND 1000000L
#define MICROS_IN_ONE_MILLI 1000L

#if defined(PARTICLE)
#define F_CPU 16000000 // definition for a board for which F_CPU is not defined
#elif defined(ARDUINO_ARCH_MBED_RP2040)
#define F_CPU 133000000
#elif defined(ARDUINO_ARDUINO_NANO33BLE)
#define F_CPU 64000000
#endif
#if defined(F_CPU)
#define CLOCKS_PER_MICRO (F_CPU / MICROS_IN_ONE_SECOND)
#else
#error F_CPU not defined, please define it for your board in IRremoteInt.h
#endif

/*
 * For backwards compatibility
 */
#if defined(SYSCLOCK) // allow for processor specific code to define F_CPU
#undef F_CPU
#define F_CPU SYSCLOCK // Clock frequency to be used for timing.
#endif

//#define DEBUG // Activate this for lots of lovely debug output from the IRremote core and all protocol decoders.
//#define TRACE // Activate this for more debug output.

/**
 * For better readability of code
 */
#define DISABLE_LED_FEEDBACK false
#define ENABLE_LED_FEEDBACK true
#define USE_DEFAULT_FEEDBACK_LED_PIN 0


/**
 * This struct contains the data and control used for receiver static functions and the ISR (interrupt service routine)
 * Only StateForISR needs to be volatile. All the other fields are not written by ISR after data available and before start/resume.
 */
struct irparams_struct {
    // The fields are ordered to reduce memory over caused by struct-padding
    volatile uint8_t StateForISR;   ///< State Machine state
    uint8_t IRReceivePin;           ///< Pin connected to IR data from detector
#if defined(__AVR__)
    volatile uint8_t *IRReceivePinPortInputRegister;
    uint8_t IRReceivePinMask;
#endif
    uint16_t TickCounterForISR;     ///< Counts 50uS ticks. The value is copied into the rawbuf array on every transition.

    bool OverflowFlag;              ///< Raw buffer OverflowFlag occurred
#if RAW_BUFFER_LENGTH <= 254        // saves around 75 bytes program space and speeds up ISR
    uint8_t rawlen;                 ///< counter of entries in rawbuf
#else
    unsigned int rawlen;            ///< counter of entries in rawbuf
#endif
    uint16_t rawbuf[RAW_BUFFER_LENGTH]; ///< raw data / tick counts per mark/space, first entry is the length of the gap between previous and current command
};


#define IRDATA_FLAGS_EMPTY              0x00
#define IRDATA_FLAGS_IS_REPEAT          0x01
#define IRDATA_FLAGS_IS_AUTO_REPEAT     0x02
#define IRDATA_FLAGS_PARITY_FAILED      0x04 ///< the current (autorepeat) frame violated parity check
#define IRDATA_TOGGLE_BIT_MASK          0x08
#define IRDATA_FLAGS_EXTRA_INFO         0x10 ///< there is unexpected extra info not contained in address and data (e.g. Kaseikyo unknown vendor ID)
#define IRDATA_FLAGS_WAS_OVERFLOW       0x40 ///< irparams.rawlen is 0 in this case to avoid endless OverflowFlag
#define IRDATA_FLAGS_IS_LSB_FIRST       0x00
#define IRDATA_FLAGS_IS_MSB_FIRST       0x80 ///< Just for info. Value is simply determined by the protocol

/**
 * Just for better readability of code
 */
#define USE_DEFAULT_FEEDBACK_LED_PIN 0

extern uint8_t sBiphaseDecodeRawbuffOffset; //

/*
 * Mark & Space matching functions
 */
bool matchTicks(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
bool matchMark(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
bool matchSpace(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);

/*
 * Old function names
 */
bool MATCH(uint16_t measured, uint16_t desired);
bool MATCH_MARK(uint16_t measured_ticks, uint16_t desired_us);
bool MATCH_SPACE(uint16_t measured_ticks, uint16_t desired_us);

int getMarkExcessMicros();

void setFeedbackLED(bool aSwitchLedOn);
void setLEDFeedback(uint8_t aFeedbackLEDPin, bool aEnableLEDFeedback); // if aFeedbackLEDPin == 0, then take board BLINKLED_ON() and BLINKLED_OFF() functions
void setLEDFeedback(bool aEnableLEDFeedback); // Direct replacement for blink13()
void enableLEDFeedback();
void disableLEDFeedback();

void setBlinkPin(uint8_t aFeedbackLEDPin) __attribute__ ((deprecated ("Please use setLEDFeedback()."))); // deprecated

/**
 * microseconds per clock interrupt tick
 */
#if ! defined(MICROS_PER_TICK)
#define MICROS_PER_TICK    50
#endif

/*
 * Pulse parms are ((X*50)-100) for the Mark and ((X*50)+100) for the Space.
 * First MARK is the one after the long gap
 * Pulse parameters in uSec
 */
/** Relative tolerance (in percent) for some comparisons on measured data. */
#define TOLERANCE       25

/** Lower tolerance for comparison of measured data */
//#define LTOL            (1.0 - (TOLERANCE/100.))
#define LTOL            (100 - TOLERANCE)
/** Upper tolerance for comparison of measured data */
//#define UTOL            (1.0 + (TOLERANCE/100.))
#define UTOL            (100 + TOLERANCE)

//#define TICKS_LOW(us)   ((int)(((us)*LTOL/MICROS_PER_TICK)))
//#define TICKS_HIGH(us)  ((int)(((us)*UTOL/MICROS_PER_TICK + 1)))
#if MICROS_PER_TICK == 50 && TOLERANCE == 25           // Defaults
#define TICKS_LOW(us)   ((us)/67 )     // (us) / ((MICROS_PER_TICK:50 / LTOL:75 ) * 100)
#define TICKS_HIGH(us)  ((us)/40 + 1)  // (us) / ((MICROS_PER_TICK:50 / UTOL:125) * 100) + 1
#else
    #define TICKS_LOW(us)   ((uint16_t) ((long) (us) * LTOL / (MICROS_PER_TICK * 100) ))
    #define TICKS_HIGH(us)  ((uint16_t) ((long) (us) * UTOL / (MICROS_PER_TICK * 100) + 1))
#endif

#define NO_REPEATS  0
#define SEND_STOP_BIT true
#define SEND_REPEAT_COMMAND true ///< used for e.g. NEC, where a repeat is different from just repeating the data.

/**
 * Duty cycle in percent for sent signals.
 */
#if ! defined(IR_SEND_DUTY_CYCLE)
#define IR_SEND_DUTY_CYCLE 30 // 30 saves power and is compatible to the old existing code
#endif

/**
 * Main class for sending IR signals
 */
class IRsend {
public:
    IRsend();

#if defined(IR_SEND_PIN) || defined(SEND_PWM_BY_TIMER)
    void begin();
#endif
#if !defined(IR_SEND_PIN) && !defined(SEND_PWM_BY_TIMER)
    IRsend(uint8_t aSendPin);
    void setSendPin(uint8_t aSendPinNumber);
#endif

    void begin(bool aEnableLEDFeedback, uint8_t aFeedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);
    // Not guarded for backward compatibility
    void begin(uint8_t aSendPin, bool aEnableLEDFeedback = true, uint8_t aFeedbackLEDPin = USE_DEFAULT_FEEDBACK_LED_PIN);

    void enableIROut(uint8_t aFrequencyKHz);

    void sendPulseDistanceWidthData(unsigned int aOneMarkMicros, unsigned int aOneSpaceMicros, unsigned int aZeroMarkMicros,
            unsigned int aZeroSpaceMicros, uint32_t aData, uint8_t aNumberOfBits, bool aMSBfirst, bool aSendStopBit = false);
    void sendBiphaseData(unsigned int aBiphaseTimeUnit, uint32_t aData, uint_fast8_t aNumberOfBits);

    void mark(unsigned int aMarkMicros);
    void space(unsigned int aSpaceMicros);
    void IRLedOff();

#if !defined(IR_SEND_PIN) && !defined(SEND_PWM_BY_TIMER)
    uint8_t sendPin;
#endif
    unsigned int periodTimeMicros;
    unsigned int periodOnTimeMicros; // compensated with PULSE_CORRECTION_NANOS for duration of digitalWrite.

    void customDelayMicroseconds(unsigned long aMicroseconds);
};

/*
 * The sender instance
 */
extern IRsend IrSender;

#endif // IRremoteInt_h

#pragma once

