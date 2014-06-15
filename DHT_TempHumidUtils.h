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

#include "math.h"


class DHT_TempHumidUtils {

	public:

		static float convertCelsiusToFahrenheit(float celsius);
		static float convertFahrenheitToCelsius(float fahrenheit);

		// Correct to +/- 1.3F when temp >= 80 and humidity >= 40; error is
		// possibly larger outside that range
		static float computeHeatIndexFahrenheit(float tempFahrenheit, float percentHumidity);

		// This has the same error as above, converted to +/- 0.7222C
		static float computeHeatIndexCelsius(float tempCelsius, float percentHumidity);


	private:

		DHT_TempHumidUtils();

		// used by the public computeHeadIndex functions
		static float computeHeatIndexRothfusz(float tempFahrenheit, float percentHumidity);

};

#endif
