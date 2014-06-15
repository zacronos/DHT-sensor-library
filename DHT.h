#ifndef DHT_H
#define DHT_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "DHT_TempHumidUtils.h"

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

// different versions of the sensor; pass one of these in as the "type"
// parameter when constructing a DHT object
#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

// how much data we want to read
#define NUM_BYTES 5

class DHT {

	public:

		DHT(uint8_t pin, uint8_t type);
		void begin();

		// depending on the sensor type, temperature is correct to within:
		//     DHT11: +/- 2.0 degrees C
		//     DHT21: +/- 1.0 degree C
		//     DHT22: +/- 0.2 degrees C
		float getTemperatureCelsius();
		float readTemperatureCelsius();

		// depending on the sensor type, temperature is correct to within:
		//     DHT11: +/- 3.60 degrees F
		//     DHT21: +/- 1.80 degrees F
		//     DHT22: +/- 0.36 degrees F
		float getTemperatureFahrenheit();
		float readTemperatureFahrenheit();

		// depending on the sensor type, percent humidity is correct to within:
		//     DHT11: +/- 5%
		//     DHT21: +/- 3% to 5%
		//     DHT22: +/- 2% to 5%
		float getPercentHumidity();
		float readPercentHumidity();

		boolean readSensorData();


	private:

		uint8_t pin_, type_;

		uint8_t minSampleDelayMillis_;
		uint8_t data_[6];
		unsigned long lastReadTime_;
		boolean firstReading_;
		boolean validData_;

		boolean prepareRead();
		int16_t timeSignalLength(uint8_t signalState);
		int8_t readBit();

};

#endif
