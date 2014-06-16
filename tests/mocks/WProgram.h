#ifndef WProgram_h
#define WProgram_h

#include <math.h>

// Constants
#define LOW		0
#define HIGH	1
#define INPUT	2
#define OUTPUT	3

// I/O functions
void pinMode(unsigned int pin, unsigned int mode);
void digitalWrite(unsigned int pin, unsigned int value);
unsigned int value digitalRead(unsigned int pin);

// time and delay functions
unsigned long millis();
unsigned long micros();
void delay(unsigned long milliseconds);
void delayMicroseconds(unsigned long microseconds);

// functions that aren't really part of WProgram.h, but are needed for mocking purposes
void setTime(unsigned long timeMillis, unsigned int remainderTimeMicros=0);
void setSensorBits(float celsius, float humidity, int bitFormat);

#endif
