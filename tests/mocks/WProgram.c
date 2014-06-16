#include "WProgram.h"
#include <math.h>

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


// internal values
static unsigned long timeMillis_ = 12345;
static unsigned long timeMicros_ = 12345678;
static unsigned int remainderTimeMicros_ = 678;

static unsigned short signals_[85];
static int durations_[85];
static int index_ = -1;
static unsigned long nextTransition_;
static unsigned long wrapFrom_ = 0;



// mock I/O functions

void pinMode(unsigned int pin, unsigned int mode) {
	delayMicroseconds(2);

	if (mode == INPUT && index_ >= 0) {
		// reset internal values to start over on sensor output values
		wrapFrom_ = micros();
		index_ = 0;
		nextTransition_ = wrapFrom_+durations_[0];
		if (wrapFrom_ <= nextTransition_) {
			wrapFrom_ = 0;
		}
	}
}

void digitalWrite(unsigned int pin, unsigned int value) {
	delayMicroseconds(5);
}

unsigned int digitalRead(unsigned int pin) {
	delayMicroseconds(5);

	unsigned long currentTime;

	currentTime = micros();

	// move through the transitions until we "find" the current time or a
	// special negative duration; a duration < 0 means stay on the current
	// value forever
	while (durations_[index_] >= 0 && (nextTransition_ < currentTime || wrapFrom_)) {
		if (wrapFrom_) {
			// if wrapFrom_ is set, then we need to see a wrap before we check
			// against the nextTransition_ value

			if (currentTime > wrapFrom_) {
				// we haven't see the wrap yet
				return signals_[index_];
			}

			// we must have wrapped; reset the wrapFrom_ value and continue
			wrapFrom_ = 0;
		}

		// move on to the next signal definition
		index_++;
		wrapFrom_ = nextTransition_;
		nextTransition_ += durations_[index_];
		// check if we need to watch for a wrap...  this could also happen if
		// the duration is < 0, but we don't care because that's a special
		// case that is handled separately anyway
		if (wrapFrom_ <= nextTransition_) {
			wrapFrom_ = 0;
		}
	}

	return signals_[index_];
}


// mock time and delay functions

unsigned long millis() {
	delayMicroseconds(1);
	return timeMillis_;
}

unsigned long micros() {
	delayMicroseconds(1);
	return timeMicros_;
}

void delay(unsigned long milliseconds) {
	timeMillis_ += milliseconds;
	timeMicros_ += milliseconds*1000;
	delayMicroseconds(1);
}

void delayMicroseconds(unsigned long microseconds) {
	timeMicros_ += microseconds;
	remainderTimeMicros_ += microseconds;
	timeMillis_ += remainderTimeMicros_/1000;
	remainderTimeMicros_ %= 1000;
}


// mock interrupt control

void interrupts() {
	delayMicroseconds(3);
	// do nothing
}
void noInterrupts() {
	delayMicroseconds(3);
	// do nothing
}


// the following functions aren't really part of WProgram.h, but are needed
// for mocking purposes in the testing program

void setTime(unsigned long timeMillis, unsigned int remainderTimeMicros) {
	timeMillis_ = timeMillis;
	timeMicros_ = timeMillis*1000 + remainderTimeMicros;
	remainderTimeMicros_ = remainderTimeMicros;
}

void setSensorValues(float celsius, float humidity, int bitFormat) {
	int j=0;
	unsigned long bits=0;
	unsigned int checksum=0;

	signals_[j] = HIGH;
	durations_[j++] = 5;
	signals_[j] = LOW;
	durations_[j++] = 80;
	signals_[j] = HIGH;
	durations_[j++] = 80;

	switch (bitFormat) {
		case 8:
			bits |= (((unsigned short)round(humidity))&0xFF)<<24;
			bits |= (((unsigned short)round(celsius))&0xFF)<<8;
			break;
		case 16:
			bits |= (((unsigned long)round(humidity*10.0))&0xFFFF)<<16;
			// negative temperatures are transmitted using 1s-complement integer
			// format, so we mask off the highest-order bit
			bits |= (((unsigned long)fabs(round(celsius*10.0)))&0x7FFF)<<0;
			// and then we need to drop in the sign bit
			bits |= (celsius < 0 ? 0x1 : 0x0)<<15;
			break;
	}

	checksum += (bits & (0xFF<<0))>>0;
	checksum += (bits & (0xFF<<8))>>8;
	checksum += (bits & (0xFF<<16))>>16;
	checksum += (bits & (0xFF<<24))>>24;
	checksum &= 0xFF;

	// set up the signals and durations needed to transmit these values out
	// properly to the Arduino
	for (int i=31; i>=0; i--) {
		signals_[j] = LOW;
		durations_[j++] = 50;
		signals_[j] = HIGH;
		durations_[j++] = (bits & (0x1<<i))?70:26;
	}
	for (int i=7; i>=0; i--) {
		signals_[j] = LOW;
		durations_[j++] = 50;
		signals_[j] = HIGH;
		durations_[j++] = (checksum & (0x1<<i))?70:26;
	}

	signals_[j] = LOW;
	durations_[j++] = -1;

	index_ = 0;
	nextTransition_ = micros()+durations_[0];
}

