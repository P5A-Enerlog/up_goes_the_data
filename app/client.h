//import required libraries
#include <WiFi.h>
#include <HTTPClient.h>

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://preprodapi.mde.epf.fr/add_measure.php";
// Service API Key
String apiKey = "a4dce674484e493f7ab1f40a7a9e6f94";

void upload_sensor(String sensorId, String sensorVal)
{
  if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "id_system_sensor=" + sensorId + "&value="+ sensorVal + "&token=" + apiKey;   
      
      // id_system_sensor=55&value=11&token=a4dce674484e493f7ab1f40a7a9e6f94
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode); 

      // Free resources
      http.end();
    }
}