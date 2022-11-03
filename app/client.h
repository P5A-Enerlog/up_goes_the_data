// import required libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Domain Name with full URL Path for HTTP POST Request
const char *serverName = "http://preprodapi.mde.epf.fr/add_measure.php";
// const char *serverName = "";
// Service API Key
String apiKey = "a4dce674484e493f7ab1f40a7a9e6f94";

void upload_sensor(String sensorId, String sensorVal)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "id_system_sensor=" + sensorId + "&value=" + sensorVal + "&token=" + apiKey;

    // id_system_sensor=55&value=11&token=a4dce674484e493f7ab1f40a7a9e6f94
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    Serial.println("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
  }
}

// Get exact time (minutes only)
int get_time()
{
  WiFiClient client;
  HTTPClient http;
  String timeZone = "Europe/Paris";
  String serverName = "https://timeapi.io/api/Time/current/";
  String httpRequest = serverName + "zone?timeZone=" + timeZone;

  http.begin(httpRequest.c_str()); // init the connection

  int minutes = 99; // default value in case it is unable to get the time
  int httpResponseCode = http.GET(); // GET request

  if (httpResponseCode==200) // in case of success
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString(); // get payload data
    Serial.println(payload);

    DynamicJsonDocument timeDoc(1024); // init json document 
    deserializeJson(timeDoc, payload); // transform payload to json

    minutes = timeDoc["minute"]; // extract the minutes value
    Serial.println(minutes);
  }
  else // request failed
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // end the connection

  return minutes;
}