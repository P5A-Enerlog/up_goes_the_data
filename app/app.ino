//import required libraries
#include "max6675.h"
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "client.h"
#include "sensors.h"

const char* ssid = "Pixel1";
const char* password = "12345678";

String sensorId= "54";
String sensorValue= "2048";

//  SENSORS //
#define FAN_PIN 14

#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int RecordTime = 3; //Define Measuring Time (Seconds)
const int SensorPin = 27;  //Define Interrupt Pin (2 for Arduino Uno)

int fanSpeed=200;
int fanCount =0;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

int InterruptCounter;
float WindSpeed;

int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;

unsigned long timerDelay = 60 * 1000 * 5; // 5 minutes timer

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  dht.begin();

  // wait for MAX chip to stabilize
  delay(500);

  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(FAN_PIN, ledChannel);

  delay(500);  
}

void loop() {
  //Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    
    upload_sensor(sensorId,sensorValue);
    
    lastTime = millis();
  }
}

void get_dht() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  } else {
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  }  
}

void get_thermocouple() {
  //thermocouple
  Serial.print("Thermocouple, C = "); 
  Serial.println(thermocouple.readCelsius());
}

void get_anemometer() {
   //anemometer
  measure();
  Serial.print("Wind Speed: ");
  Serial.print(WindSpeed);       //Speed in km/h
  Serial.print(" km/h - ");
  Serial.print(WindSpeed / 3.6); //Speed in m/s
  Serial.println(" m/s"); 
}

void set_fan() {
  //fan
  fanSpeed = (fanCount%2==0)*255;
  ledcWrite(ledChannel, fanSpeed);
  Serial.print("fan speed: ");
  Serial.println(fanSpeed);
  fanCount++;
}

void measure() {
  InterruptCounter = 0;
  attachInterrupt(digitalPinToInterrupt(SensorPin), countup, RISING);
  delay(1000 * RecordTime);
  detachInterrupt(digitalPinToInterrupt(SensorPin));
  WindSpeed = (float)InterruptCounter / (float)RecordTime * 2.4;
}

void countup() {
  InterruptCounter++;
}