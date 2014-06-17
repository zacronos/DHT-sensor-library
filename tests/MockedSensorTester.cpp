// Testing program that uses a mocked Arduino environment to check library
// functionality and correctness
//
// Written by Joe Ibershoff, released under MIT license

#include "math.h"
#include "WProgram.h"
#include "DHT.h"
#include "stdio.h"


void testCode(float celsius_, float humidity_, DHT &dht, unsigned short valueWidth) {
	setSensorValues(celsius_, humidity_, valueWidth);

	float humidity = dht.readPercentHumidity();
	float temperatureC = dht.readTemperatureCelsius();
	float temperatureF = dht.readTemperatureFahrenheit();

	// Check if any reads failed
	if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
		printf("Failed to read from DHT sensor!\n");
		return;
	}

	// Compute heat index
	float heatIndexF = dht.readHeatIndexFahrenheit();
	float heatIndexC = dht.readHeatIndexCelsius();

	printf("Humidity: %4.1f%%     Temperature: %5.1f*C / %5.1f*F     Heat index: %5.1f*C / %5.1f*F\n", humidity, temperatureC, temperatureF, heatIndexC, heatIndexF);
}

int main(int argc, char** argv) {

	DHT dht16(2, DHT22);
	dht16.begin();

	// check the output of these functions to see if they match the input
	// use this to check the heat index values:
	//     http://www.hpc.ncep.noaa.gov/html/heatindex.shtml

	testCode(37.1, 30.4, dht16, 16);
	delay(2000);
	testCode(-38.7, 9.2, dht16, 16);
	delay(2000);
	testCode(30.2, 75.3, dht16, 16);
	delay(2000);
	printf("\n");

	// these should yield pretty much the same thing as above, but we'll lose
	// the 10ths precision and the negative temperature will be all funky

	DHT dht8(2, DHT11);
	dht8.begin();
	testCode(37.1, 30.4, dht8, 8);
	delay(2000);
	testCode(-38.7, 9.2, dht8, 8);
	delay(2000);
	testCode(30.2, 75.3, dht8, 8);
}
