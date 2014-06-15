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
 ***************************************************************************/

// how many timing transitions we need to keep track of. 2 * number bits + extra
#define MAXTIMINGS 85

#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

class DHT {
	private:
		uint8_t pin_, type_, count_;

		uint8_t minSampleDelayMillis_;
		uint8_t data_[6];
		uint32_t lastReadTime_;
		boolean firstReading_;

	public:
		DHT(uint8_t pin, uint8_t type, uint8_t count=6);
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

		boolean read();
};

#endif
