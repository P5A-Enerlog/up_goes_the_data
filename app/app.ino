//import required libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include "client.h"

const char* ssid = "Pixel1";
const char* password = "12345678";

String sensorId= "54";
String sensorValue= "2048";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;

unsigned long timerDelay = 60 * 1000 * 5;

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
  
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

}

void loop() {
  //Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    
    upload_sensor(sensorId,sensorValue);
    
    lastTime = millis();
  }
}