#ifndef DHT_H
#define DHT_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

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
		uint8_t _pin, _type, _count;

		uint8_t data[6];
		unsigned long _lastreadtime;
		boolean firstreading;

		float computeHeatIndexRothfusz(float tempFahrenheit, float percentHumidity);

	public:
		DHT(uint8_t pin, uint8_t type, uint8_t count=6);
		void begin();

		float getTemperatureCelsius();
		float getTemperatureFahrenheit();
		float readTemperatureCelsius();
		float readTemperatureFahrenheit();

		float convertCelsiusToFahrenheit(float celsius);
		float convertFahrenheitToCelsius(float fahrenheit);

		float computeHeatIndexFahrenheit(float tempFahrenheit, float percentHumidity);
		float computeHeatIndexCelsius(float tempCelsius, float percentHumidity);
		float readHumidity();

		boolean read();
};
#endif
