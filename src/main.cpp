#include <Arduino.h>
#include <WiFi.h>
#include <stdbool.h>
#include <vector>
#include "Alberto_mqtt.hpp"
#include "json_Deserialize.hpp"
#include <iostream>
#include <Alberto_time.hpp>
#include <Wire.h>
#include <epaper.hpp>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "time.h"
#include <iostream>
#include <string>
#include <ArduinoHttpClient.h>
// #include <curl/curl.h>
// #include <jsoncpp/json/json.h>

/* SSID and password of your WiFi net ----------------------------------------*/
const char *ssid = "H@cka1r0n"; //"your ssid";
const char *password = "!H@cka1h0n!";   //"your password";

//deep sleep defines
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600 // 5 minuten        /* Time ESP32 will go to sleep (in seconds) */

//Raum defines
#define Raumname "Java"
#define Raumnummer "D2/16"

IPAddress myIP;        // IP address in your local wifi net

Meeting_struct x;

void setup() {
  Serial.begin(115200);

  // Applying SSID and password
   WiFi.begin(ssid, password);

   // Waiting the connection to a router
   while (WiFi.status() != WL_CONNECTED) {
       delay(500);
       Serial.print(".");
   }
  Serial.println(myIP = WiFi.localIP());
 
 Serial.println(WiFi.dnsIP()); 

 // mqtt_setup();
 // mqtt_connect();

  epaper_setup();
  time_main();
   
  x.organizer_name = "Jamo";
  x.subject = "M100 Implementation";
  x.start_timestamp = 1676269565;
  x.end_timestamp = 1676319965;
  x.sensitivity = "testSens";
  v_Meetings.emplace_back(x);
}

void getJson() 
{
  vector<Meeting_struct> v_Meetings;
  Meeting_struct weather;

  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, "api.openweathermap.org", 80);

  //client.beginRequest();
  
  std::cout << ">>>>>>>>>>>>>>>>" << client.get("/data/2.5/weather?q=London,uk&APPID=72bf310c823f2e48cb85ef2294b583cc") << "\n"; // data/2.5/weather?q=London,uk&APPID=72bf310c823f2e48cb85ef2294b583ccl") 
  //client.endRequest();

  std::cout << ">>>>>>>>>>>>>>>>" << client.responseStatusCode() << "\n";
  String payload = client.responseBody();

  JSON_Value *root_value;
  JSON_Object *root_object;

  JSON_Array *weather_array;
  JSON_Object *weather_object;
  size_t i;

  root_value = json_parse_string(payload.c_str());
  root_object = json_value_get_object(root_value);

  weather_array = json_object_get_array(root_object, "weather");

  for(i = 0; i < json_array_get_count(weather_array); i++) {
      weather_object = json_array_get_object(weather_array, i);
      weather.subject = string(json_object_get_string(weather_object, "description"));
      //v_Meetings.emplace_back(weather);
      std::cout << "#############jsonData############ : " << weather.subject << "\n";
    }
}

void loop() 
{
  // mqtt_loop();

  getJson();
    
  for (size_t i = 0; i < v_Meetings.size(); i++)
  {
    std::cout << "Meeting " << i << ":\n";
    std::cout << "    " << "Subject: " << v_Meetings[i].subject << "\n";
    printf("    Start: %ld\n", v_Meetings[i].start_timestamp);
    printf("    End: %ld\n", v_Meetings[i].end_timestamp);
    std::cout << "    " << "Organizer_name: " << v_Meetings[i].organizer_name << "\n";
    std::cout << "    " << "Sensitivity: " << v_Meetings[i].sensitivity << "\n";
  }
  
  time_t t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream s; 
  s << put_time(&tm, "%d.%m.%Y");
  string date = s.str();
  
  calender_text(Raumname, Raumnummer, date);
  
  delay(30000);
  
  //deep sleep 
  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // esp_deep_sleep_start();
}