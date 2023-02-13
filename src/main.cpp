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

/* SSID and password of your WiFi net ----------------------------------------*/
const char *ssid = "H@cka1r0n"; //"your ssid";
const char *password = "!H@cka1h0n!";   //"your password";

//deep sleep defines
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600 // 5 minuten        /* Time ESP32 will go to sleep (in seconds) */

//Raum defines
#define Raumname "HAllO"
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

   
  x.organizer_name = "Marc";
  x.subject = "test";
  x.start_timestamp = 1676269565;
  x.end_timestamp = 1676319965;
  x.sensitivity = "testSens";
  v_Meetings.emplace_back(x);

}

void loop() {
  // mqtt_loop();
    
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

  
  delay(15000);
  
  //deep sleep 
  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // esp_deep_sleep_start();
}