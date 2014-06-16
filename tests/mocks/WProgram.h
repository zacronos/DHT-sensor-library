#ifndef WProgram_h
#define WProgram_h


/***************************************************************************
 * special-purpose mock Arduino environment
 * written by Joe Ibershoff
 * distributed under MIT license
 *
 * The purpose of this header and associated module is to allow use of the
 * DHT class outside of an actual Arduino, by mocking up the constants and
 * functions normally provided by the Arduino environment via either
 * WProgram.h or Arduino.h.  This can be modified to help mock other classes,
 * but it is special-purpose written to do what DHT needs; it probably won't
 * work as-is for mocking other libraries
 ***************************************************************************/


#ifndef boolean
#define boolean short int
#endif
#include "stdint.h"


// mock constants
#define LOW		0
#define HIGH	1
#define INPUT	2
#define OUTPUT	3

// mock I/O functions
void pinMode(unsigned int pin, unsigned int mode);
void digitalWrite(unsigned int pin, unsigned int value);
unsigned int digitalRead(unsigned int pin);

// mock interrupt control
void interrupts();
void noInterrupts();

// mock time and delay functions
unsigned long millis();
unsigned long micros();
void delay(unsigned long milliseconds);
void delayMicroseconds(unsigned long microseconds);


// the following functions aren't really part of WProgram.h, but are needed
// for mocking purposes in the testing program

// setTime() sets the current time value as returned by both millis() and
// micros().  All of the mock functions increment the time by at least one
// millisecond, with values somewhat arbitrarily hardcoded (except the delay
// functions, of course).
void setTime(unsigned long timeMillis, unsigned int remainderTimeMicros=0);

// this sets the values the sensor will return, and in addition takes a
// bitFormat parameter that should be either 8 or 16, to indicate the width
// of the values returned from the sensor (8 for DHT11, or 16 for DHT2*)
void setSensorValues(float celsius, float humidity, int bitFormat);

#endif
