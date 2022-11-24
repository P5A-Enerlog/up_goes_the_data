#include "max6675.h"
#include "DHT.h"
int InterruptCounter;

// dht
String get_thermocouple(MAX6675 thermocouple)
{
    float thmcple = thermocouple.readCelsius();
    char sensorValue[50];
    sprintf(sensorValue, "%g", thmcple);
    
    return sensorValue;
}

// get temperature if sensor=0, else get humidity
String get_dht(DHT dht, int sensor)
{
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float v;
    if (sensor)
    {
      v = dht.readHumidity();
    } else {
      // Read temperature as Celsius (the default)
      v = dht.readTemperature();
    }
    
    char sensorValue[50];
    sprintf(sensorValue, "%g", v);

    return sensorValue;
}

void countup()
{
  InterruptCounter++;
}

String get_anemometer(int RecordTime, int SensorPin)
{
  // anemometer
  InterruptCounter = 0;
  attachInterrupt(digitalPinToInterrupt(SensorPin), countup, RISING);
  delay(1000 * RecordTime);
  detachInterrupt(digitalPinToInterrupt(SensorPin));
  float WindSpeed = (float)InterruptCounter / (float)RecordTime * 2.4;

  char sensorValue[50];
  sprintf(sensorValue, "%g", WindSpeed);

  Serial.print("Wind Speed: ");
  Serial.print(WindSpeed); // Speed in km/h
  Serial.print(" km/h - ");
  Serial.print(WindSpeed / 3.6); // Speed in m/s
  Serial.println(" m/s");

  return sensorValue;
}

String get_pyrano(int analogPin)
{
  float pyr = analogRead(analogPin);
  pyr = pyr * ((0.4 * 3300) / 4095); // ESP32 conversion : 0-4096 -> 0-3.3Vs

  char sensorValue[50];
  sprintf(sensorValue, "%g", pyr);
  
  return sensorValue;
}
