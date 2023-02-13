#include <MQTT.h>
#include <WiFi.h>
#include <string>
#include <cstring>



/* MQTT and Wifi "objects"----------------------------------------*/
extern WiFiClient net;
extern MQTTClient client;

extern unsigned long lastMillis;
extern string json_payload;

void mqtt_connect();
void mqtt_setup();
void mqtt_loop();

