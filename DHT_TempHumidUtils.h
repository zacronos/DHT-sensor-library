#ifndef DHT_TEMP_HUMID_UTILS_H
#define DHT_TEMP_HUMID_UTILS_H

/***************************************************************************
 * Temperature and humidity utility functions, written as part of:
 * https://github.com/zacronos/DHT-sensor-library
 * written by Joe Ibershoff
 * distributed under MIT license
 *
 * based in part on the library originally written by Adafruit Industries
 * https://github.com/adafruit/DHT-sensor-library
 ***************************************************************************/

static class DHT_TempHumidUtils {
	private:
		static float computeHeatIndexRothfusz(float tempFahrenheit, float percentHumidity);

	public:
		static float convertCelsiusToFahrenheit(float celsius);
		static float convertFahrenheitToCelsius(float fahrenheit);

		static float computeHeatIndexFahrenheit(float tempFahrenheit, float percentHumidity);
		static float computeHeatIndexCelsius(float tempCelsius, float percentHumidity);
};

#endif
