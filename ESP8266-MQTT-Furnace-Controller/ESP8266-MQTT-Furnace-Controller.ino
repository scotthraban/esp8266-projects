#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/*
 * Inside your MyConfig.h file, you will need to define the params to connect
 * to your Wifi and MQTT server:
 * 
 * #define WIFI_SSID "Your SSID"
 * #define WIFI_PWD "Your-Password"
 * #define MQTT_SERVER "your.server.com"
 * #define MQTT_PORT your-mqtt-port
 * #define MQTT_CLIENT_ID "your-client-id"
 * #define MQTT_USERNAME "your-mqtt-username"
 * #define MQTT_PWD "your-mqtt-password
 * #define MQTT_TOPIC "your-mqtt-topic"
 */
#include "MyConfig.h"
 
#define TRANSISTOR_OUT_PIN 2 // Pin where transistor output is connected
#define FAILSAFE_TIMEOUT 180000 // Timeout (millis) at which furnace is turned off if no
                                // messages have been received

WiFiClient espClient;
PubSubClient client(espClient);

long lastCommandReceived = 0;

void setup()
{
  pinMode(TRANSISTOR_OUT_PIN, OUTPUT);

  setSwitch(0);

  startWifi();
 
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(processMessage);
}
 
void loop()
{
  long now = millis();
  if (now > (lastCommandReceived + FAILSAFE_TIMEOUT)) {
    setSwitch(0);
  }
  
  if (!client.connected()) {
    if (reconnect()) {
      // (re)connect success, subscribe to topic, then loop again
      client.subscribe(MQTT_TOPIC);
    } else {
      // (re)connect failure, sleep and try again later
      delay(5000);
    }
  } else {
    // connected - process messages, sleep a bit and go around again
    client.loop();
    delay(500);
  }
}

void processMessage(char* topic, byte* payload, unsigned int length)
{
  if (strncmp(MQTT_TOPIC, topic, strlen(MQTT_TOPIC)) != 0) {
    // this is not the topic you are looking for
    return;
  }

  if (length == 2 && strncmp("ON", (char*)payload, 2) == 0) {
    setSwitch(1);
    lastCommandReceived = millis();
  } else if (length == 3 && strncmp("OFF", (char*)payload, 3) == 0) {
    setSwitch(0);
    lastCommandReceived = millis();
  }
}

void setSwitch(int status)
{
  if (status == 1) {
    digitalWrite(TRANSISTOR_OUT_PIN, HIGH);
  } else {
    digitalWrite(TRANSISTOR_OUT_PIN, LOW);
  }
}

boolean reconnect()
{
#ifdef MQTT_USERNAME
  return client.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
#else
  return client.connect(MQTT_CLIENT_ID);
#endif
}

void startWifi()
{
  delay(50);

  // Ensure that ESP8266 only starts up in Station mode
  WiFi.mode(WIFI_STA);

  // Set the hostname, reusing the mqtt client id
  WiFi.hostname(MQTT_CLIENT_ID);

  WiFi.begin(WIFI_SSID, WIFI_PWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
