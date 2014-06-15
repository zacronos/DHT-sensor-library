/***************************************************************************
 * DHT sensor library
 * https://github.com/zacronos/DHT-sensor-library
 * (re)written by Joe Ibershoff
 * distributed under MIT license
 *
 * based on the library originally written by Adafruit Industries
 * https://github.com/adafruit/DHT-sensor-library
 *
 * fix for readings shortly after a failure based on Glenn Ramsey's fork
 * https://github.com/glennra/DHT-sensor-library
 ***************************************************************************/

#include "DHT.h"

DHT::DHT(uint8_t pin, uint8_t type) {
	pin_ = pin;
	type_ = type;
	firstReading_ = true;
	validData_ = false;

	switch (type_) {
		case DHT11:
			minSampleDelayMillis_ = 1000;
			break;
		case DHT22:
		case DHT21:
			minSampleDelayMillis_ = 2000;
			break;
	}
}

void DHT::begin() {
	// set up the pins!
	pinMode(pin_, INPUT);
	digitalWrite(pin_, HIGH);
	lastReadTime_ = 0;
}

boolean DHT::readSensorData() {
	uint8_t byteIndex, bitIndex;
	unsigned long currentTime;
	uint8_t bit;

	// Check if sensor was read in the last sample window, and if so return
	// early to use the values from the last reading
	currentTime = millis();
	if (currentTime < lastReadTime_) {
		// i.e. there was a rollover
		lastReadTime_ = 0;
	}
	if (!firstReading_ && ((currentTime - lastReadTime_) < minSampleDelayMillis_)) {
		// we're not going to ask the sensor for more data, so just return a
		// value indicating whether the data currently in the buffer is valid
		return validData_;
	}
	firstReading_ = false;

	// clear the buffer, disable interrupts, signal the sensor, etc
	if (!prepareRead()) {
		// something's wrong; turn interrupts back on and bail
		interrupts();
		return false;
	}

	// get our data bits: they come out high-order bits first, so we have to
	// write them into the buffer "backwards"
	for (byteIndex = NUM_BYTES-1; byteIndex >= 0; byteIndex--) {
		for (bitIndex = 7; bitIndex >= 0; bitIndex--) {
			bit = readBit();
			if (bit == -1) {
				// that didn't work; turn interrupts back on and bail
				interrupts();
				return false;
			}
			// write the bit into the appropriate location in the buffer
			data_[byteIndex] |= (bit<<bitIndex);
		}
	}

	// turn interrupts back on
	interrupts();

	// test for data validity: data_[4] is a checksum byte, and should equal
	// the low byte of the sum of the other 4 bytes
	validData_ = (data_[4] == ((data_[0] + data_[1] + data_[2] + data_[3]) & 0xFF));

	return validData_;
}

float DHT::getTemperatureCelsius() {
	float temperature;

	if (!validData_) {
		return NAN;
	}

	// different versions of the sensor yield data in different formats
	switch (type_) {
		case DHT11:
			// data is in whole degrees, and fits in a byte, convenient!
			return data_[2];
		case DHT22:
		case DHT21:
			// mask the sign bit off data_[2], then shift it left 8 bits,
			// and drop data_[3] into the low-order byte
			temperature = ((data_[2] & 0x7F) << 8) ^ data_[3];
			// put the correct sign on the float value
			if (data_[2] & 0x80) {
				temperature *= -1;
			}
			// raw data is in tenths of degrees, so scale the result
			return temperature/10.0;
	}
	return NAN;
}

float DHT::getTemperatureFahrenheit() {
	return DHT_TempHumidUtils::convertCelsiusToFahrenheit(getTemperatureCelsius());
}

float DHT::getPercentHumidity() {
	float humidity;

	if (!validData_) {
		return NAN;
	}

	switch (type_) {
		case DHT11:
			// data is in whole percents, and fits in a byte, convenient!
			return data_[0];
		case DHT22:
		case DHT21:
			// shift data_[0] left 8 bits, and drop data_[1] into the low-order byte
			humidity = (data_[0] << 8) ^ data_[1];
			// raw data is in tenths of a percent, so scale the result
			return humidity/10.0;
	}
	return NAN;
}


float DHT::readTemperatureCelsius() {
	// read in raw data, and check for failure
	if (!readSensorData()) {
		return NAN;
	}
	return getTemperatureCelsius();
}

float DHT::readTemperatureFahrenheit() {
	return DHT_TempHumidUtils::convertCelsiusToFahrenheit(readTemperatureCelsius());
}

float DHT::readPercentHumidity() {
	// read in raw data, and check for failure
	if (!readSensorData()) {
		return NAN;
	}
	return getPercentHumidity();
}

float DHT::readHeatIndexFahrenheit() {
	// read in raw data, and check for failure
	if (!readSensorData()) {
		return NAN;
	}
	return DHT_TempHumidUtils::computeHeatIndexFahrenheit(getTemperatureFarenheit(), getRelativeHumidity());
}

float DHT::readHeatIndexCelsius(){
	// read in raw data, and check for failure
	if (!readSensorData()) {
		return NAN;
	}
	return DHT_TempHumidUtils::computeHeatIndexCelsius(getTemperatureCelsius(), getRelativeHumidity());
}


boolean DHT::prepareRead() {
	int16_t digitalReadCycles;

	// record current time
	lastReadTime_ = millis();

	// blank out the buffer
	data_[0] = data_[1] = data_[2] = data_[3] = data_[4] = 0;
	
	// set flag to show we haven't gotten valid data from this read
	validData_ = false;

	// pull the pin high and wait 250 milliseconds for the sensor to chill out
	digitalWrite(pin_, HIGH);
	delay(250);

	// now pull it low for ~20 milliseconds as the start signal
	pinMode(pin_, OUTPUT);
	digitalWrite(pin_, LOW);
	delay(20);

	// turn off interrupts before continuing further
	noInterrupts();

	// then pull it high for ~40 microseconds
	digitalWrite(pin_, HIGH);
	delayMicroseconds(40);

	// now we're ready to read the response signal before the data
	pinMode(pin_, INPUT);

	// first watch for the sensor to transition away from HIGH
	if (timeSignalLength(HIGH) == -1) {
		// we've failed to initialize properly
		return false;
	}

	// the sensor will keep the pin LOW for ~80 microseconds
	if (timeSignalLength(LOW) == -1) {
		// we've failed to initialize properly
		return false;
	}

	// then it will bring the pin HIGH for another ~80 microseconds
	if (timeSignalLength(HIGH) == -1) {
		// we've failed to initialize properly
		return false;
	}

	// we're done preparing, the next LOW-HIGH cycle will be a real bit
	return true;
}

int16_t DHT::timeSignalLength(uint8_t signalState) {
	unsigned long currentTimeMicros;
	unsigned long startTimeMicros = micros();
	unsigned long timeWaiting;

	while (digitalRead(pin_) == signalState) {
		// watch how long we've been waiting
		currentTimeMicros = micros();
		// handle overflows
		timeWaiting = (currentTimeMicros > startTimeMicros ?
				currentTimeMicros - startTimeMicros :
				ULONG_MAX - startTimeMicros + currentTimeMicros );
		if (timeWaiting > 200) {
			// there is a problem; the sensor should never leave us hanging
			// for more than 80 microseconds, and even on devices with a
			// micros() resolution of 8 microseconds, that means we shouldn't
			// see an apparent delay of longer than 96 microseconds... but
			// we'll cut it some slack and wait for up to 200 microseconds
			return -1;
		}
		delayMicroseconds(1);
	}

	// get the final observed wait
	currentTimeMicros = micros();
	// handle overflows
	timeWaiting = ( currentTimeMicros > startTimeMicros ?
			currentTimeMicros - startTimeMicros :
			ULONG_MAX - startTimeMicros + currentTimeMicros );
	return (uint16_t)timeWaiting;
}

int8_t DHT::readBit() {
	int16_t signalLength;

	// first, we should get a LOW signal for ~50 microseconds
	signalLength = timeSignalLength(LOW);
	if (signalLength == -1) {
		// we never saw the end of the signal, oops
		return -1;
	}

	// then, we should get a HIGH signal for:
	//     ~26-28 microseconds for a "0" bit
	//     ~70 microseconds for a "1" bit
	signalLength = timeSignalLength(HIGH);
	if (signalLength == -1) {
		// we never saw the end of the signal, oops
		return -1;
	}

	// we'll count < 50 microseconds as a "0", and everything else as a "1"
	return (signalLength < 50)? 0 : 1;
}
