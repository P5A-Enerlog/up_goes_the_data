#include "max6675.h"
#include "DHT.h"
int InterruptCounter;

// dht
void get_thermocouple(MAX6675 thermocouple, String sensorValue, String sensorId)
{
    sensorValue = thermocouple.readCelsius() + '0';

    upload_sensor(sensorId, sensorValue);

    Serial.print("Thermocouple, C = ");
    Serial.println(sensorValue);
}

void get_dht(DHT dht, String sensorValue, String sensorId)
{
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);


    sensorValue = t + '0';

    //upload_sensor(sensorId, sensorValue);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
    }
    else
    {
        // Compute heat index in Fahrenheit (the default)
        float hif = dht.computeHeatIndex(f, h);
        // Compute heat index in Celsius (isFahreheit = false)
        float hic = dht.computeHeatIndex(t, h, false);

        Serial.print(F("%  Temperature: "));
        Serial.print(t);
        Serial.println(F("°C "));
    }
}

void countup()
{
  InterruptCounter++;
}

void get_anemometer(int RecordTime, int SensorPin, String sensorValue, String sensorId)
{
  // anemometer
  InterruptCounter = 0;
  attachInterrupt(digitalPinToInterrupt(SensorPin), countup, RISING);
  delay(1000 * RecordTime);
  detachInterrupt(digitalPinToInterrupt(SensorPin));
  float WindSpeed = (float)InterruptCounter / (float)RecordTime * 2.4;

  sensorValue = WindSpeed + '0';
  upload_sensor(sensorId, sensorValue);

  Serial.print("Wind Speed: ");
  Serial.print(WindSpeed); // Speed in km/h
  Serial.print(" km/h - ");
  Serial.print(WindSpeed / 3.6); // Speed in m/s
  Serial.println(" m/s");
}

void get_pyrano(int analogPin, String sensorValue, String sensorId)
{
  float pyr = analogRead(analogPin);
  pyr = pyr * ((0.4 * 3300) / 4095); // ESP32 conversion : 0-4096 -> 0-3.3Vs

  // Set the sensor id for upload
  sensorValue = pyr + '0';
  upload_sensor(sensorId, sensorValue);

  Serial.println(pyr);
}
