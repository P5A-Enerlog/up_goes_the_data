// import required libraries
#include "max6675.h"
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "client.h"
#include "sensors.h"

const char *ssid = "Pixel1";
const char *password = "12345678";

String sensorId = "";
String sensorValue = "";

//  SENSORS //
#define FAN_PIN 14

#define analogPin 2 // pin de sortie du pyranomètre

#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int RecordTime = 3; // Define Measuring Time (Seconds)
const int SensorPin = 27; // Define Interrupt Pin (2 for Arduino Uno)

float pyr = 0; // initialisation du pyranomètre

int fanSpeed = 200;
int fanCount = 0;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

float WindSpeed;

int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;

// unsigned long timerDelay = 60 * 1000 * 5; // 5 minutes timer
unsigned long timerDelay = 10 * 1000; // 5 minutes timer

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
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

  set_fan(0);

  delay(500);
}

void loop()
{
  // Send an HTTP POST request every 5 min
  if ((millis() - lastTime) > timerDelay)
  {

    get_thermocouple(thermocouple, sensorValue, "60");

    //Serial.println("thermo is done");
    delay(100);

    get_dht(dht, sensorValue, "54");
    //Serial.println("dht is done");
    delay(100);

    get_anemometer(RecordTime, SensorPin, sensorValue, "53");
    //Serial.println("anemo is done");

    lastTime = millis();
  }
}

void set_fan(int fanSpeed)
{
  // fan
  ledcWrite(ledChannel, fanSpeed);
  Serial.print("fan speed: ");
  Serial.println(fanSpeed);
}
