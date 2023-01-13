# Software for the Enerlog Smart Heating Project

All documentation can be found on the [wiki](https://github.com/P5A-Enerlog/up_goes_the_data/wiki)

## Overview
This code is made for an ESP32 Arduino board  
The system is able to:  
- Collect data from 8 sensors  
- Send it to an [API](https://preprodapi.mde.epf.fr/measure.php?id=60) (hosed on EPF servers)
- Send it at a custom time interval (minimum every 5 min)
- Automatically synchronize time by calling an external [time API](https://timeapi.io/)
- Restart the connection if it is unable to connect, restart the whole system if it is for a too long time   

## Secrets variables
Another file must to be added that is not on the repositiry because it constains secrets variables. You have to create a `secret_keys.h` file and fill it with:
```
#define EPF_API_KEY "an api key"
#define WIFI_SSID "network name"
#define WIFI_PASSWORD "network password"
```
Replacing the strings with your values

## Files
Other than the `secret_keys.h`, here are the files provided here:
- `app.ino` contains the main behaviour of the ESP32
- `client.h` contains the functions related to WiFi connection
- `sensors.h` contains the functions related to sensors 
