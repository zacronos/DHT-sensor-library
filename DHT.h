#ifndef DHT_H
#define DHT_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "limits.h"
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
#define DHT11  11
#define DHT21  21
#define AM2301 21
#define DHT22  22
#define AM2303 22

// how much data we want to read
#define NUM_BYTES 5

class DHT {

	public:

		// constructor and initializer
		DHT(uint8_t pin, uint8_t type);
		void begin();

		// readSensorData() does the actual magic of reading data from the
		// sensor and storing them in the buffer, returning a flag indicating
		// whether the data in the buffer is valid.  If readSensorData() is
		// called more than once in less time than is allowed for the sensor,
		// readSensorData() will not attempt to read from the sensor again,
		// and instead will just return the flag value last returned.
		//
		// If you want to have full control over what happens when, you will
		// want to use this function and the get*() functions; otherwise, you
		// can just use the convenience read*() functions.
		boolean readSensorData();

		// the get*() functions read the data from the buffer, and in the case
		// of getTemperatureFahrenheit(), converts the value from Celsius.  If
		// any of these functions returns NAN, then you will have to try again
		// after the minimum sampling delay has passed
		//
		// depending on the sensor type, temperature is correct to within:
		//     DHT11: +/- 2.0 degrees C
		//     DHT21: +/- 1.0 degree C
		//     DHT22: +/- 0.2 degrees C
		//
		// depending on the sensor type, temperature is correct to within:
		//     DHT11: +/- 3.60 degrees F
		//     DHT21: +/- 1.80 degrees F
		//     DHT22: +/- 0.36 degrees F
		//
		// depending on the sensor type, percent humidity is correct to within:
		//     DHT11: +/- 5%
		//     DHT21: +/- 3% to 5%
		//     DHT22: +/- 2% to 5%
		//
		float getTemperatureCelsius();
		float getTemperatureFahrenheit();
		float getPercentHumidity();

		// the read*() functions are convenience functions that encapsulate a
		// readSensorData() call followed by any get*() and conversions needed
		float readTemperatureCelsius();
		float readTemperatureFahrenheit();
		float readPercentHumidity();

		// note that there are no corresponding getHeatIndex*() functions;
		// this is because the heat index is a derived value, and is not
		// actually present in the data buffers.
		//
		// See DHT_TempHumidUtils.h for notes on the accuracy of the heat
		// index calculation, but also keep in mind that the error in the
		// temperature and percentHumidity values will be compounded when
		// passed through the heat index calculation, so the overall error
		// bounds is larger than any of the individual error bounds
		float readHeatIndexCelsius();
		float readHeatIndexFahrenheit();


	private:

		uint8_t pin_, type_;

		uint16_t minSampleDelayMillis_;
		uint8_t data_[6];
		unsigned long lastReadTime_;
		boolean firstReading_;
		boolean validData_;

		boolean prepareRead();
		int16_t timeSignalLength(uint8_t signalState);
		int8_t readBit();

};

#endif
