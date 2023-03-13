#include "json_Deserialize.hpp"
#include <WiFi.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <epaper.hpp>
#include <Alberto_time.hpp>
#include <ctime>
#include <iomanip>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SPIFFS.h>
#include <string>

vector<Meeting_struct> v_Meetings;

void JsonSetup(const char **ssid, const char **password, string Raumname, string Raumnummer,string roomEmail)
{
    JSON_Value *root_value;
    JSON_Array *json_array;
    JSON_Object *json_object;
    String payload;
  

    if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  File file = SPIFFS.open("/Hawaii.txt");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  
  Serial.println("File Content:");
  
  
    payload = file.readString();
    Serial.println(payload);
    root_value = json_parse_string(payload.c_str());
    json_object = json_value_get_object(root_value);
    

    *ssid = json_object_get_string(json_object, "SSID");
    *password = json_object_get_string(json_object, "Password");
    Raumname = json_object_get_string(json_object, "RoomName");
    Raumnummer = json_object_get_string(json_object, "RoomNumber");
    roomEmail = json_object_get_string(json_object, "EmailAddress");
   // std::cout << "SSID: " << ssid << "\n";
   // std::cout << "PASSWORD: " << password << "\n";
    std::cout << "ROOMNAME: " << Raumname << "\n";
    std::cout << "ROOMNUMBER: " << Raumnummer << "\n";
    std::cout << "ROOMEMAIL: " << roomEmail << "\n";
}