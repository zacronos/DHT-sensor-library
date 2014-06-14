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
	_pin = pin;
	_type = type;
	_count = count;
	firstreading = true;
}

void DHT::begin(void) {
	// set up the pins!
	pinMode(_pin, INPUT);
	digitalWrite(_pin, HIGH);
	_lastreadtime = 0;
}

// convenience function to combine read() and getTemperatureCelsius()
float DHT::readTemperatureCelsius() {
	// read in raw data, and check for failure
	if (!read()) {
		return NAN;
	}
	return getTemperatureCelsius();
}

// get temperature value from raw data buffer
float DHT::getTemperatureCelsius() {
	float temperature;

	// different versions of the sensor yield data in different formats
	switch (_type) {
		case DHT11:
			// data is in whole degrees, and fits in a byte, convenient!
			return data[2];
		case DHT22:
		case DHT21:
			// mask the sign bit off data[2], then shift it left 8 bits,
			// and drop data[3] into the low-order byte
			temperature = ((data[2] & 0x7F) << 8) ^ data[3];
			// put the correct sign on the float value
			if (data[2] & 0x80) {
				temperature *= -1;
			}
			// raw data is tenths of degrees, so scale the result
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

float DHT::readHumidity(void) {
	float f;
	if (read()) {
		switch (_type) {
		case DHT11:
			f = data[0];
			return f;
		case DHT22:
		case DHT21:
			f = data[0];
			f *= 256;
			f += data[1];
			f /= 10;
			return f;
		}
	}
	return NAN;
}

float DHT::computeHeatIndexRothfusz(float tempFahrenheit, float percentHumidity) {
	// TODO: do I need to declare this private here, or just in the .h?
	// Adapted from the "Rothfusz regression" equation at:
	//     https://github.com/adafruit/DHT-sensor-library/issues/9
	// and
	//     Wikipedia: http://en.wikipedia.org/wiki/Heat_index
	//
	// This formula computes the heat index +/- 1.3F vs the heat index table
	// provided by the NOAA, but is only considered accurate for temperatures
	// >= 80F and relative humidities >= 40%; in other words, this only works
	// in warm, humid weather.
	//
	// The NOAA also notes that "The Rothfusz regression is not valid for
	// extreme temperature and relative humidity conditions", which in context
	// seems to indicate this equation is inaccurate at very high temperatures
	// as well, but doesn't go into detail on those limitations.

	float tempFahrenheitSquared = pow(tempFahrenheit, 2);
	float percentHumiditySquared = pow(percentHumidity, 2);

	return -42.379 + 
			 2.04901523 * tempFahrenheit +
			10.14333127 * percentHumidity +
			-0.22475541 * tempFahrenheit * percentHumidity +
			-0.00683783 * tempFahrenheitSquared +
			-0.05481717 * percentHumiditySquared +
			 0.00122874 * tempFahrenheitSquared * percentHumidity +
			 0.00085282 * tempFahrenheit * percentHumiditySquared +
			-0.00000199 * tempFahrenheitSquared * percentHumiditySquared;
}

boolean DHT::read(void) {
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;
	unsigned long currenttime;

	// Check if sensor was read less than two seconds ago and return early
	// to use last reading.
	currenttime = millis();
	if (currenttime < _lastreadtime) {
		// ie there was a rollover
		_lastreadtime = 0;
	}
	if (!firstreading && ((currenttime - _lastreadtime) < 2000)) {
		return true; // return last correct measurement
		//delay(2000 - (currenttime - _lastreadtime));
	}
	firstreading = false;
	/*
		Serial.print("Currtime: "); Serial.print(currenttime);
		Serial.print(" Lasttime: "); Serial.print(_lastreadtime);
	*/
	_lastreadtime = millis();

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;
	
	// pull the pin high and wait 250 milliseconds
	digitalWrite(_pin, HIGH);
	delay(250);

	// now pull it low for ~20 milliseconds
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	delay(20);
	noInterrupts();
	digitalWrite(_pin, HIGH);
	delayMicroseconds(40);
	pinMode(_pin, INPUT);

	// read in timings
	for ( i=0; i< MAXTIMINGS; i++) {
		counter = 0;
		while (digitalRead(_pin) == laststate) {
			counter++;
			delayMicroseconds(1);
			if (counter == 255) {
				break;
			}
		}
		laststate = digitalRead(_pin);

		if (counter == 255) break;

		// ignore first 3 transitions
		if ((i >= 4) && (i%2 == 0)) {
			// shove each bit into the storage bytes
			data[j/8] <<= 1;
			if (counter > _count)
				data[j/8] |= 1;
			j++;
		}

	}

	interrupts();
	
	/*
	Serial.println(j, DEC);
	Serial.print(data[0], HEX); Serial.print(", ");
	Serial.print(data[1], HEX); Serial.print(", ");
	Serial.print(data[2], HEX); Serial.print(", ");
	Serial.print(data[3], HEX); Serial.print(", ");
	Serial.print(data[4], HEX); Serial.print(" =? ");
	Serial.println(data[0] + data[1] + data[2] + data[3], HEX);
	*/

	// check we read 40 bits and that the checksum matches
	if ((j >= 40) && 
			(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
		return true;
	}
	

	return false;

}
