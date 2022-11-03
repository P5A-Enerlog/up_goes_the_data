#include "max6675.h"
#include "DHT.h"
int InterruptCounter;

// dht
void get_thermocouple(MAX6675 thermocouple, String sensorValue, String sensorId)
{
    float thmcple = thermocouple.readCelsius();
    char c[50];
    sprintf(c, "%g", thmcple);
    sensorValue = c; 
    
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

    char c[50];
    sprintf(c, "%g", t);
    sensorValue = c; 

    upload_sensor(sensorId, sensorValue);
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

  char c[50];
  sprintf(c, "%g", WindSpeed);
  sensorValue = c; 

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

  char c[50];
  sprintf(c, "%g", pyr);
  sensorValue = c; 
  upload_sensor(sensorId, sensorValue);

  Serial.println(pyr);
}
