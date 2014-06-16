#include "WProgram.h"




// internal values
static unsigned long timeMillis_ = 12345;
static unsigned long timeMicros_ = 12345678;
static unsigned int remainderTimeMicros_ = 678;

static unsigned short signals_[85];
static int durations_[85];
static int index_ = -1;
static unsigned long nextTransition_;
static unsigned long wrapFrom_ = 0;



// functions that aren't really part of WProgram.h, but are needed for mocking purposes

void setTime(unsigned long timeMillis, unsigned int remainderTimeMicros) {
	timeMillis_ = timeMillis;
	timeMicros_ = timeMillis*1000 + remainderTimeMicros;
	remainderTimeMicros_ = remainderTimeMicros;
}

void setSensorBits(unsigned long bits) {
	int j=0;
	unsigned int checksum=0;

	signals_[j] = HIGH;
	durations_[j++] = 5;
	signals_[j] = LOW;
	durations_[j++] = 80;
	signals_[j] = HIGH;
	durations_[j++] = 80;

	checksum += bits & (0xFF<<0);
	checksum += bits & (0xFF<<8);
	checksum += bits & (0xFF<<16);
	checksum += bits & (0xFF<<24);
	checksum &= 0xFF;

	for (int i=7; i>=0; i--) {
		signals_[j] = LOW;
		durations_[j++] = 50;
		signals_[j] = HIGH;
		durations_[j++] = (checksum & (0x1<<i))?70:26;
	}
	for (int i=31; i>=0; i--) {
		signals_[j] = LOW;
		durations_[j++] = 50;
		signals_[j] = HIGH;
		durations_[j++] = (bits & (0x1<<i))?70:26;
	}
}


// I/O functions

void pinMode(unsigned int pin, unsigned int mode) {
	delayMicroseconds(2);
	if (mode == INPUT && index_ >= 0) {
		wrapFrom_ = micros();
		index_ = 0;
		nextTransition_ += durations_[0];
		if (wrapFrom_ <= nextTransition_) {
			wrapFrom_ = 0;
		}
	}
}

void digitalWrite(unsigned int pin, unsigned int value) {
	delayMicroseconds(5);
}

unsigned int value digitalRead(unsigned int pin) {
	unsigned long currentTime;

	delayMicroseconds(5);

	currentTime = micros();

	// move through the transitions until we "find" the current time or a
	// special negative duration; a duration < 0 means stay on the current
	// value forever
	while (durations_[index_] >= 0 && (nextTransition_ < currentTime || wrapFrom_) {
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


// time and delay functions

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
