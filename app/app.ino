// import required libraries
#include "max6675.h"
#include <HTTPClient.h>
#include "client.h"
#include "sensors.h"

//  SENSORS //
#define DHT_PIN_1 27
#define DHT_PIN_2 26
#define DHT_PIN_3 25
#define DHT_PIN_4 33
#define DHT_TYPE DHT22
DHT dht1(DHT_PIN_1, DHT_TYPE);
DHT dht2(DHT_PIN_2, DHT_TYPE);
DHT dht3(DHT_PIN_3, DHT_TYPE);
DHT dht4(DHT_PIN_4, DHT_TYPE);
DHT *dhts[4]={&dht1, &dht2, &dht3, &dht4};

#define THERMOCOUPLE_SO 19
#define THERMOCOUPLE_CLK 18
#define THERMOCOUPLE_CS_1 5
#define THERMOCOUPLE_CS_2 23
MAX6675 thermocouple1(THERMOCOUPLE_CLK, THERMOCOUPLE_CS_1, THERMOCOUPLE_SO);
MAX6675 thermocouple2(THERMOCOUPLE_CLK, THERMOCOUPLE_CS_2, THERMOCOUPLE_SO);
MAX6675 *thermocouples[2]={&thermocouple1, &thermocouple2};

// TODO adjust variables types (short) to optimize memory

#define ANEMO_PIN 32 // anemometer pin 
const int anemoRecordTime = 10; // Define Measuring Time for anemometer (Seconds)

#define PYRANO_PIN 2 // analog pin for pyranometer
float pyrano = 0; // init pyranometer

#define FAN_PIN 14
int fanSpeed = 0; // No more than 191
int fanCount = 0;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
float WindSpeed;

// OTHER VARIABLES //
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;

const int number_of_sensors = 12;
// fill sensor_Ids with values corresponding to the sensors IDs in the EPF API
// DHT1-H DHT1-T DHT2-H DHT2-T DHT3-H DHT3-T DHT4-H DHT4-T Thermocouple1 Thermocouple2 Pyranometer Anemometer
// int haut - int bas - entrée haut - sortie bas - 
const int sensor_Ids[] = {57, 58, 59, 60, 67, 68, 69, 70 ,61, 62, 52, 53};
String sensor_values[number_of_sensors];

void setup()
{
  // TODO modify EPF API to add anemometer

  pinMode(PYRANO_PIN, INPUT);


  Serial.begin(9600);
  delay(500);
  
  wifi_start();

  dht1.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();

  delay(500);

  // init fan control
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(FAN_PIN, ledChannel);

  set_fan(0);////

  delay(500);
}

// Time management in minutes, example for TIME_INTERVAL=30:
// 1         // 25            28 // 30
// 31        // 55            58 // 0
// idle time // preparation time // send time


void loop()
{
  int minutes = get_time(0)%TIME_INTERVAL; // a value between 0 and TIME_INTERVAL-1 
  //Serial.print("Minutes: ");
  //Serial.println(minutes);
  // preparation time: between 4 and 1 minutes before send time
  if ( minutes >= TIME_INTERVAL-4 && minutes <= TIME_INTERVAL-1 )
  {
    wifi_stop();

  // get all the sensors values
  // 57 upper air layer humidity
  // 58 upper air layer temp
  // 59 lower air layer humidity
  // 60 lower air layer temp 
  // 67 upper tube humidity
  // 68 upper tube temp
  // 69 lower tube humidity
  // 70 lower tube temp 
  // 61 upper surface temp 
  // 62 lower surface temp 
  // 52 pyranometer 
  // 53 anemometer
    
    // DHTs
    int k=0;
    for(int i=0; i<8; i+=2){
      sensor_values[i] = get_dht(*dhts[k], 1); // get humidity
      delay(2500);
      sensor_values[i+1] = get_dht(*dhts[k], 0); // get temperature
      delay(2500);

      //Serial.print("DHT ");
      //Serial.println(k);
      //Serial.print("H = ");
      //Serial.println(sensor_values[i]);
      //Serial.print("T = ");
      //Serial.println(sensor_values[i+1]);
      k++;
    }

    // Thermocouples
    for(int i=8; i<10; i++){
      k=i-8;
      sensor_values[i] = get_thermocouple(*thermocouples[k]);
      delay(100);
      if (i==8 && sensor_values[i].toFloat()>=22.0)
      {
        fanSpeed = 150;
      } else {
        fanSpeed = 0;
      }
      //Serial.print("Thermocouple ");
      //Serial.print(k);
      //Serial.print(" = ");
      //Serial.println(sensor_values[i]);
    }

    set_fan(fanSpeed);

    // Pyranometer
    sensor_values[10] = get_pyrano(PYRANO_PIN);
    delay(100);
    //Serial.print("Pyrano: ");
    //Serial.println(sensor_values[10]);

    // Anemometer
    sensor_values[11] = get_anemometer(anemoRecordTime, ANEMO_PIN);
    delay(100);

    // Reconnect to WiFi
    wifi_start();

    // now that all the data is collected, wait until next send time
    //Serial.println("Waiting until next send time...");
    wait_until_next_send_time(0);

   
    // send the data
    //Serial.println("Send time!");
    for (int i=0; i<number_of_sensors; i++) //i<number_of_sensors
    {
      ////Serial.println("Sensor is fakely send");
      ////Serial.print("");
      ////Serial.println(sensor_values[i]);
      upload_sensor(sensor_Ids[i], sensor_values[i]);
      //Serial.print(sensor_Ids[i]);
      //Serial.println(" is uploaded");
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
  //Serial.print("fan speed: ");
  //Serial.println(fanSpeed);
}
