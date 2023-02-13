#include "Alberto_mqtt.hpp"
#include <iostream>
#include "json_Deserialize.hpp"
#include <epaper.hpp>

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;
string json_payload;

using namespace std;
void mqtt_connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("Hawaii")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  //client.subscribe("Meeting/_BZD2_16.Hawaii");
  client.subscribe("Meeting/Privat");
  // client.unsubscribe("/hello");
}


void mqtt_messageReceived(string &topic, string &payload) {
  //cout << "incoming: " << topic << " - " << payload << "\n";
  
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
  
  
  json_Deserialize(payload);
 
  //std::cout << "Json_payload after cpoing from payload: " << json_payload << "\n";
}

void mqtt_setup()
{
    // client.begin("10.204.0.12", net);
    // client.onMessage(mqtt_messageReceived);
}

void mqtt_loop()
{
  // client.loop();
  // delay(10);  // <- fixes some issues with WiFi stability

  // if (!client.connected()) {
  //   mqtt_connect();
  // }

}