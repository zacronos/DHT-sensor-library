/***************************************************************************
 * DHT sensor library
 * https://github.com/zacronos/DHT-sensor-library
 * (re)written by Joe Ibershoff
 * distributed under MIT license
 *
 * based on the library originally written by Adafruit Industries
 * https://github.com/adafruit/DHT-sensor-library
 ***************************************************************************/

#include "DHT.h"

DHT::DHT(uint8_t pin, uint8_t type, uint8_t count) {
	pin_ = pin;
	type_ = type;
	count_ = count;
	firstReading_ = true;

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

// convenience function to combine read() and getTemperatureCelsius()
float DHT::readTemperatureCelsius() {
	// read in raw data, and check for failure
	if (!read()) {
		return NAN;
	}
	return getTemperatureCelsius();
}

// get temperature value from raw data_ buffer
float DHT::getTemperatureCelsius() {
	float temperature;

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
			temperature /= 10;

			return temperature;
	}
}

// convenience function for Fahrenheit
float DHT::readTemperatureFahrenheit() {
	// will this work correctly when readTemperatureCelsius() returns NAN?
	// TODO: refresh memory on math involving NAN, and possibly fix here
	return convertCelsiusToFahrenheit(readTemperatureCelsius());
}

// convenience function for Fahrenheit
float DHT::getTemperatureFahrenheit() {
	return convertCelsiusToFahrenheit(getTemperatureCelsius());
}

// convenience function to combine read() and getPercentHumidity()
float DHT::readPercentHumidity() {
	// read in raw data, and check for failure
	if (!read()) {
		return NAN;
	}
	return getPercentHumidity();
}

float DHT::getPercentHumidity() {
	float humidity;

	switch (type_) {
		case DHT11:
			// data is in whole percents, and fits in a byte, convenient!
			return data_[0];
		case DHT22:
		case DHT21:
			// shift data_[0] left 8 bits, and drop data_[1] into the low-order byte
			humidity = (data_[0] << 8) ^ data_[1];
			// raw data is in tenths of a percent, so scale the result
			humidity /= 10;

			return humidity;
	}
	return NAN;
}

boolean DHT::read() {
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;
	uint32_t currenttime;

	// Check if sensor was read less than two seconds ago and return early
	// to use last reading.
	currenttime = millis();
	if (currenttime < lastReadTime_) {
		// i.e. there was a rollover
		lastReadTime_ = 0;
	}
	if (!firstReading_ && ((currenttime - lastReadTime_) < minSampleDelayMillis_)) {
		return true; // return last correct measurement
	}
	firstReading_ = false;
	/*
		Serial.print("Currtime: "); Serial.print(currenttime);
		Serial.print(" Lasttime: "); Serial.print(lastReadTime_);
	*/
	lastReadTime_ = millis();

	data_[0] = data_[1] = data_[2] = data_[3] = data_[4] = 0;
	
	// pull the pin high and wait 250 milliseconds
	digitalWrite(pin_, HIGH);
	delay(250);

	// now pull it low for ~20 milliseconds
	pinMode(pin_, OUTPUT);
	digitalWrite(pin_, LOW);
	delay(20);
	noInterrupts();
	digitalWrite(pin_, HIGH);
	delayMicroseconds(40);
	pinMode(pin_, INPUT);

	// read in timings
	for ( i=0; i< MAXTIMINGS; i++) {
		counter = 0;
		while (digitalRead(pin_) == laststate) {
			counter++;
			delayMicroseconds(1);
			if (counter == 255) {
				break;
			}
		}
		laststate = digitalRead(pin_);

		if (counter == 255) break;

		// ignore first 3 transitions
		if ((i >= 4) && (i%2 == 0)) {
			// shove each bit into the storage bytes
			data_[j/8] <<= 1;
			if (counter > count_)
				data_[j/8] |= 1;
			j++;
		}

	}

	interrupts();
	
	/*
	Serial.println(j, DEC);
	Serial.print(data_[0], HEX); Serial.print(", ");
	Serial.print(data_[1], HEX); Serial.print(", ");
	Serial.print(data_[2], HEX); Serial.print(", ");
	Serial.print(data_[3], HEX); Serial.print(", ");
	Serial.print(data_[4], HEX); Serial.print(" =? ");
	Serial.println(data_[0] + data_[1] + data_[2] + data_[3], HEX);
	*/

	// check we read 40 bits and that the checksum matches
	if ((j >= 40) && 
			(data_[4] == ((data_[0] + data_[1] + data_[2] + data_[3]) & 0xFF)) ) {
		return true;
	}
	

	return false;

}
