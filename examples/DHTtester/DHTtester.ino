// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 
  dht.begin();
}

void loop() {
  // Wait a couple seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readPercentHumidity();
  // Read temperature as Celsius
  float temperatureC = dht.readTemperatureCelsius();
  // Read temperature as Fahrenheit
  float temperatureF = dht.readTemperatureFahrenheit();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  float heatIndexF = dht.readHeatIndexFahrenheit();
  float heatIndexC = dht.readHeatIndexCelsius();

  Serial.print("Humidity: "); 
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(temperatureC);
  Serial.print(" *C / ");
  Serial.print(temperatureF);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(heatIndexC);
  Serial.println(" *C / ");
  Serial.print(heatIndexF);
  Serial.println(" *F");
}
