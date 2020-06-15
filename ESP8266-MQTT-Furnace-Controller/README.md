# ESP8266 MQTT over Wifi furnace controller

This project is written to be compiled from the Arduino IDE for the ESP8266.

There are a number of parameters that will be specific to the IOT environment you are installing the controller into.

To configure, put a number of #define entries into a MyConfig.h file in the same directory as the project file:

```
#define WIFI_SSID "Your SSID"
#define WIFI_PWD "Your-Password"
#define MQTT_SERVER "your.server.com"
#define MQTT_PORT your-mqtt-port
#define MQTT_CLIENT_ID "your-client-id"
#define MQTT_USERNAME "your-mqtt-username"
#define MQTT_PWD "your-mqtt-password
#define MQTT_TOPIC "your-mqtt-topic"
```

Better would be to break up the common MQTT and WiFi params into their own files and put those into you Arduino IDE path, so that you can reuse them, and then you just need to include those two files in the MyConfig.h files, and specify the MQTT client id and topic, since those would be specific to each controller you build.
