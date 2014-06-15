/***************************************************************************
 * Temperature and humidity utility functions, written as part of:
 * https://github.com/zacronos/DHT-sensor-library
 * written by Joe Ibershoff
 * distributed under MIT license
 *
 * based in part on the library originally written by Adafruit Industries
 * https://github.com/adafruit/DHT-sensor-library
 ***************************************************************************/

#include "DHT_TempHumidUtils.h"

DHT_TempHumidUtils::DHT_TempHumidUtils() {}

/* static */ float DHT_TempHumidUtils::convertCelsiusToFahrenheit(float celsius) {
	return celsius * 1.8 + 32;
}

/* static */ float DHT_TempHumidUtils::convertFahrenheitToCelsius(float fahrenheit) {
	return (fahrenheit - 32) * 0.5555555555555556;
}

/* static */ float DHT_TempHumidUtils::computeHeatIndexFahrenheit(float tempFahrenheit, float percentHumidity) {
	// Correct to +/- 1.3F when temp >= 80 and humidity >= 40; error is
    // possibly larger outside that range
	//
	// Adapted from equations at:
	//     http://www.hpc.ncep.noaa.gov/html/heatindex_equation.shtml
	// and javascript code from:
	//     view-source:http://www.hpc.ncep.noaa.gov/html/heatindex.shtml
	// The notable differences between the two are:
	//     1) The javascript short-circuits heatIndex = temparature for
	//        temperatures <= 40F, where the NOAA text doesn't mention this
	//        shortcut.  The shortcut is used in the code below.
	//     2) The javascript uses the Rothfusz regression when the simple heat
	//        index is > 79.0, while the NOAA text indicates >= 80.0.  The
	//        code below uses the logic from the javascript.
	//     3) The javascript rounds resulting heat index values to the nearest
	//        whole degree (F or C).  We don't do this, and instead leave it
	//        to the user to manage rounding and significant figures.
	//
	// There are some alternative equations available at:
	//     http://en.wikipedia.org/wiki/Heat_index#Formula


	float heatIndex;

	if (tempFahrenheit <= 40.0) {
		// presumably it's cold enough that not much humidity is even possible
		return tempFahrenheit;
	}

	// Start with a basic calculation, simplified from the original equation:
	//     0.5 * (tempFahrenheit + (61.0+((tempFahrenheit-68.0)*1.2)+(percentHumidity*0.094)))
	heatIndex = 1.1*tempFahrenheit + 0.047*percentHumidity - 10.3;

	if (heatIndex <= 79.0) {
		// the basic calculation is actually more accurate than the Rothfusz
		// calculation in this temperature range
		return heatIndex;
	}

	heatIndex = computeHeatIndexRothfusz(tempFahrenheit, percentHumidity);

	if (percentHumidity < 13.0 && tempFahrenheit >= 80 && tempFahrenheit <= 112) {
		// an adjustment is made for some very low-humidity conditions
		return heatIndex + (percentHumidity-13.0)/4.0 * sqrt((17.0-fabs(tempFahrenheit-95.0))/17.0);
	}

	if (percentHumidity > 85.0 && tempFahrenheit >= 80 && tempFahrenheit <= 87) {
		// an adjustment is made for some very high-humidity conditions
		return heatIndex + (percentHumidity-85.0)/10.0 * (87.0-tempFahrenheit)/5.0;
	}

	return heatIndex;
}

/* static */ float DHT_TempHumidUtils::computeHeatIndexCelsius(float tempCelsius, float percentHumidity) {
	// Correct to +/- 0.7222C when temp >= 80 and humidity >= 40; error is
    // possibly larger outside that range
	return convertFahrenheitToCelsius(computeHeatIndexFahrenheit(convertCelsiusToFahrenheit(tempCelsius), percentHumidity));
}

float DHT_TempHumidUtils::computeHeatIndexRothfusz(float tempFahrenheit, float percentHumidity) {
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
