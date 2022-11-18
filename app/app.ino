// import required libraries
#include "max6675.h"
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "client.h"
#include "sensors.h"

//  SENSORS //
#define DHT_PIN_1 32
#define DHT_PIN_2 33
#define DHT_PIN_3 25
#define DHT_PIN_4 26
#define DHT_TYPE DHT22
DHT dht1(DHT_PIN_1, DHT_TYPE);
DHT dht2(DHT_PIN_2, DHT_TYPE);
DHT dht3(DHT_PIN_3, DHT_TYPE);
DHT dht4(DHT_PIN_4, DHT_TYPE);
DHT *dhts[2]={&dht1, &dht2};

#define THERMOCOUPLE_SO 19
#define THERMOCOUPLE_CLK 18
#define THERMOCOUPLE_CS_1 5
#define THERMOCOUPLE_CS_2 23
MAX6675 thermocouple1(THERMOCOUPLE_CLK, THERMOCOUPLE_CS_1, THERMOCOUPLE_SO);
MAX6675 thermocouple2(THERMOCOUPLE_CLK, THERMOCOUPLE_CS_2, THERMOCOUPLE_SO);

// TODO adjust variables types (short) to optimize memory

#define ANEMO_PIN 27 // anemometer pin 
const int RecordTime = 3; // Define Measuring Time for anemometer (Seconds)

#define PYRANO_PIN 2 // analog pin for pyranometer
float pyrano = 0; // init pyranometer

#define FAN_PIN 14
int fanSpeed = 200;
int fanCount = 0;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
float WindSpeed;

// OTHER VARIABLES //
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;

const int number_of_sensors = 12;
int sensor_Ids[number_of_sensors];
String sensor_values[number_of_sensors];

void setup()
{
  // TODO modify EPF API to add anemometer
  // fill sensor_Ids with values corresponding to the sensors IDs in the EPF API
  //for (int i=0; i<number_of_sensors; i++)
  sensor_Ids[0] = 58;
  sensor_Ids[1] = 68;
  //sensor_Ids[2] = 60;
  //sensor_Ids[3] = 70;


  pinMode(PYRANO_PIN, INPUT);


  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  dht1.begin();
  dht2.begin();
  //dht3.begin();
  //dht4.begin();

  delay(500);

  // init fan control
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(FAN_PIN, ledChannel);

  set_fan(200);////

  delay(500);
}

// Time management in minutes, example for TIME_INTERVAL=30:
// 1         // 25            28 // 30
// 31        // 55            58 // 0
// idle time // preparation time // send time


void loop()
{
  int minutes = get_time(0)%TIME_INTERVAL; // a value between 0 and TIME_INTERVAL-1 
  Serial.print("Minutes: ");
  Serial.println(minutes);
  // preparation time: between 4 and 1 minutes before send time
  if ( minutes >= TIME_INTERVAL-4 && minutes <= TIME_INTERVAL-1 )
  {
    // get all the sensors values
    // 52 pyrano
    // 57 // 67 DHT air layer upper humidity
    // 58 // 68 DHT air layer upper temperature
    // 59 // 69 DHT air layer lower humidity
    // 60 // 70 DHT air layer lower temperature
    // 61 // 71 thermocouple outside surface upper temperature
    // 62 // 72 thermocouple outside surface lower temperature

    // TODO set all the ids

    // sensor_values[0] = get_pyrano(PYRANO_PIN);
    // delay(100);
    
    // for each dht :
    for(int i=0; i<2; i++){
      sensor_values[i] = get_dht(*dhts[i], 0);
      Serial.print("DHT");
      Serial.print(i);
      Serial.print(" = ");
      Serial.println(sensor_values[i]);
      delay(100);
    }

    // sensor_values[9] = get_thermocouple(thermocouple1);
    // delay(100);
    // sensor_values[10] = get_thermocouple(thermocouple2);
    // delay(100);

    // sensor_values[11] = get_anemometer(RecordTime, ANEMO_PIN);
    // delay(100);

    // lastTime = millis();


    // now that all the data is collected, wait until next send time
    Serial.println("Waiting until next send time...");
    wait_until_next_send_time(0);
    // send the data
    Serial.println("Send time!");
    for (int i=0; i<2; i++) //i<number_of_sensors
    {
      upload_sensor(sensor_Ids[i], sensor_values[i]);
    }
  } 
  else // idle time
  {
    delay(1*60*1000); // Wait 1 min in case the current time is just before a send time
    wait_until_next_send_time(3); // wait until 3 min before next send time (preparation time)
  }
}

void set_fan(int fanSpeed)
{
  // fan
  ledcWrite(ledChannel, fanSpeed);
  Serial.print("fan speed: ");
  Serial.println(fanSpeed);
}
