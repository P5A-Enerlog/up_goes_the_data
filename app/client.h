// import required libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secret_keys.h"

#define TIME_INTERVAL 5 // time between two send (in minutes), minimum 5

// Domain Name with full URL Path for HTTP POST Request
String serverName = "http://192.168.139.27/add_measure.php"; //"http://preprodapi.mde.epf.fr/add_measure.php"; // http://192.168.139.27/add_measure.php"
// const char *serverName = "";
// Service API Key
String apiKey = EPF_API_KEY;

void wifi_start()
{
  int count=0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(500);
  //Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //Serial.print(".");
    // Restart system if unable to connect for around 8 minutes
    if (count>8*60*2){
      ESP.restart();
    }
    // Retry to connect if unable to connect for around 2 minutes
    if (count%(2*60*2)==0){
      WiFi.disconnect();
      delay(500);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    count++;
  }
  //Serial.println("");
  //Serial.print("Connected to WiFi network with IP Address: ");
  //Serial.println(WiFi.localIP());
}

void wifi_restart()
{
  WiFi.mode(WIFI_STA);
  wifi_start();
}

void wifi_stop()
{
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  //Serial.println("wifi stopped");
}

void upload_sensor(int sensorId, String sensorVal)
{
  int count=0;
  // ensure that the connection is still estabilshed , else retry to connect
  while (WiFi.status() != WL_CONNECTED)
  {
    wifi_restart();
  } 
  
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName.c_str());

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Data to send with HTTP POST
  String httpRequestData = "id_system_sensor=" + String(sensorId) + "&value=" + sensorVal + "&token=" + apiKey;
  //Serial.print("POST ");
  //Serial.println(httpRequestData.substring(0, httpRequestData.length()-30));

  // id_system_sensor=55&value=11&twifi_restartoken=EPF_API_KEY
  // Send HTTP POST request

  // in case of 10 successive failures to connect to the api, restart the ESP
  while (http.POST(httpRequestData)!=200)
  {
    if (count>10)
    { 
      ESP.restart();
    }
    count++;
    delay(1000);
  }

  // Free resources
  http.end();
}

// Get exact time from an external API (min or sec)
// input: get_seconds indicates if you want the value in seconds (1) or in minutes (0)
// output: the value of minutes (min) or minutes*60+seconds (sec)
int get_time(int get_seconds)
{
  WiFiClient client;
  HTTPClient http;
  String timeZone = "Europe/Paris";
  String serverName = "https://timeapi.io/api/Time/current/";
  String httpRequest = serverName + "zone?timeZone=" + timeZone;
  int count = 0;
  int minutes = 99; // default value in case it is unable to get the time
  int seconds = 0; 

  // ensure that the connection is still estabilshed , else retry to connect
  while (WiFi.status() != WL_CONNECTED)
  {
    wifi_restart();
  } 

  http.begin(httpRequest.c_str()); // init the connection

  // in case of 10 successive failures to connect to the api, restart the ESP
  while (http.GET()!=200)
  {
    if (count>10)
    { 
      ESP.restart();
    }
    count++;
    delay(1000);
  }

  String payload = http.getString(); // get payload data
  //Serial.println(payload);

  DynamicJsonDocument timeDoc(1024); // init json document 
  deserializeJson(timeDoc, payload); // transform payload to json

  minutes = timeDoc["minute"]; // extract the minutes value
  if (get_seconds)
  {
    seconds = timeDoc["seconds"]; // extract the seconds value
  }

  http.end(); // end the connection

  return minutes+(minutes*59*get_seconds)+seconds; // return time in minutes if get_seconds=0, or in seconds if get_seconds=1
}

// Get the next time value to send the data
// input: current_time in seconds between 0 and 59*60
// output: next_send_time in seconds (for example, for TIME_INTERVAL=30, either 30*60 or 60*60)
int get_next_send_time(int current_time){ 
  int next_send_time = current_time;
  next_send_time += TIME_INTERVAL*60 - current_time%(TIME_INTERVAL*60); // round current time to next send time
  //Serial.print("Next send time: ");
  //Serial.println(next_send_time/60);
  return next_send_time;
}


// Wait until next send time (or x minutes before, actually x-1 min and 50 sec)
// input: before indicates how many minutes before send time
void wait_until_next_send_time(int before)
{ 
  int current_time = get_time(1); // in seconds
  int send_time = get_next_send_time(current_time)-before*60; // in seconds
  int wait_time = send_time-current_time+10; // difference between next send time and current time, in seconds, add 10 seconds for safety
  delay(wait_time*1000); // wait
}