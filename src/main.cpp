#include <Arduino.h>
#include <WiFi.h>
#include <stdbool.h>
#include <vector>
#include <json_Deserialize.hpp>
#include <iostream>
#include <Alberto_time.hpp>
#include <Wire.h>
#include <epaper.hpp>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <time.h>
#include <iostream>
#include <string>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SPIFFS.h>

using namespace std;

const char *ssid; // "your ssid";
const char *password;   // "your password";
string roomEmail;
// deep sleep defines
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

#define TIME_TO_SLEEP  30 // 1 minute(n)        /* Time ESP32 will go to sleep (in seconds) */


// Raum defines
string Raumname;
string Raumnummer;

// IP address in your local wifi net
IPAddress myIP;        
Meeting_struct x;



void setup() {
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
    

    ssid = json_object_get_string(json_object, "SSID");
    password = json_object_get_string(json_object, "Password");
    Raumname = json_object_get_string(json_object, "RoomName");
    Raumnummer = json_object_get_string(json_object, "RoomNumber");
    roomEmail = json_object_get_string(json_object, "EmailAddress");
   // std::cout << "SSID: " << ssid << "\n";
   // std::cout << "PASSWORD: " << password << "\n";
    std::cout << "ROOMNAME: " << Raumname << "\n";
    std::cout << "ROOMNUMBER: " << Raumnummer << "\n";
    std::cout << "ROOMEMAIL: " << roomEmail << "\n";
  
  Serial.begin(115200);

   // Applying SSID and password
   WiFi.begin(ssid, password);

   // Waiting the connection to a router
   while (WiFi.status() != WL_CONNECTED) 
   {
       delay(500);
       Serial.print(".");
   }
  Serial.println(myIP = WiFi.localIP());
 
  Serial.println(WiFi.dnsIP()); 

  epaper_setup();
  time_main();
}

time_t getGraphTimeAsTimestamp(string datetime) 
{
  tm tm;   
  strptime(datetime.c_str(), "%Y-%m-%dT%H:%M:%S.%f", &tm);

  return mktime(&tm);// + 60 * 60;
}



 void json_DeserializeUser(string payload, Meeting_struct jsonMeetingValue, string startTime)

{ 
  JSON_Value *root_value;
  JSON_Array *json_array;
  JSON_Object *json_object;
  string jsonStartTime; 
  string jsonName;

  root_value = json_parse_string(payload.c_str());

  json_object = json_value_get_object(root_value); // json object = full json
  json_array = json_object_get_array(json_object, "value"); //json array = value array

  json_object = json_array_get_object(json_array, 0); //json object = value array (value) 

  
  jsonName = json_object_get_string(json_object, "scheduleId"); //Get name
  jsonName = jsonName.substr(0, jsonName.size() - 14);

  json_array = json_object_get_array(json_object, "scheduleItems"); // json array = scheduleItemsArray in Value array

  for (size_t i = 0; i < json_array_get_count(json_array); i++) // für jedes scheduleitems array in value array
  {
    json_object = json_array_get_object(json_array, i);
    

    if (json_object_get_boolean(json_object, "isPrivate") == 1)
    {
      
      json_object = (json_object_get_object(json_object, "start")); //Get Date
      jsonStartTime = json_object_get_string(json_object, "dateTime");
      jsonStartTime = jsonStartTime.substr(0, jsonStartTime.size() - 8);


      if (jsonStartTime == startTime)
      {
        

        jsonMeetingValue.organizer_name = jsonName;
        jsonMeetingValue.subject = "Privater Termin";
        
        v_Meetings.emplace_back(jsonMeetingValue);
      } 
    }
    else
    {
      if (!string(json_object_get_string(json_object, "location")).compare( jsonMeetingValue.location))
      {   

        jsonMeetingValue.organizer_name = jsonName;
        jsonMeetingValue.subject = string(json_object_get_string(json_object, "subject"));   

        v_Meetings.emplace_back(jsonMeetingValue);
      } 
      
    } 

    

  }
  json_value_free(root_value);
}

// Is getting Json from attached URL
void getJson() 
{
  vector<Meeting_struct> v_Token;
  Meeting_struct token;
  JSON_Value *root_value;
  JSON_Array *json_array;
  JSON_Object *json_object;
  Meeting_struct jsonObject;
  string startTime;
  string endTime;
  WiFiClientSecure wifi;
  HTTPClient client;

  

  const char* fingerprint = \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
  "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
  "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
  "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
  "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
  "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
  "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
  "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
  "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
  "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
  "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
  "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
  "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
  "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
  "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
  "-----END CERTIFICATE-----\n";

  
  client.begin(wifi, "https://login.microsoftonline.com/e3bace4d-d2e7-4d8f-afb8-152509ee3f1a/oauth2/v2.0/token");
  client.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  wifi.setCACert(fingerprint);

  wifi.setInsecure();
  wifi.setTimeout(1000);

  String postData = "client_id=e29d3f70-8b7b-43a9-a2a2-ce5f0d7fdfef&scope=https%3A%2F%2Fgraph.microsoft.com%2F.default&client_secret=2S78Q~a4Kq~DswvIVfk6i_m3BvDL4cRWh1qq3deN&grant_type=client_credentials";

  client.POST(postData);

  string tempString = client.getString().substring(79).c_str();
  string jsonToken = tempString.substr(0, tempString.size() - 2);

  client.begin(wifi, "https://graph.microsoft.com/v1.0/users/marc.sahler@bachmann.info/calendar/getSchedule");

  client.addHeader("Authorization", ("Bearer " + jsonToken).c_str());
  client.addHeader("Content-Type", "application/json");
  client.addHeader("Prefer", "outlook.timezone=\"Europe/Berlin\"");
    
  std::cout << getTime() << "\n";

  client.POST(("{\"schedules\": [\""+ roomEmail + "\"], \"startTime\": { \"dateTime\": \"" + getTimejson() + "T07:30:00\", \"timeZone\": \"Europe/Berlin\" },\"endTime\": { \"dateTime\": \"" + getTimejson() + "T20:00:00\", \"timeZone\": \"Europe/Berlin\"  }, \"availabilityViewInterval\": 60}").c_str());
  
  string payload = client.getString().c_str();

  //JSON DESIRIALIZE
  root_value = json_parse_string(payload.c_str());
  json_object = json_value_get_object(root_value);// json object = full json
  json_array = json_object_get_array(json_object, "value"); //json array = value array

  json_object = json_array_get_object(json_array, 0); //json object = value array (value) 
  json_array = json_object_get_array(json_object, "scheduleItems"); // json array = scheduleItemsArray in Value array

  JSON_Object *tempObject;

  for (size_t i = 0; i < json_array_get_count(json_array); i++) // für jedes scheduleitems array in value array
  {
    json_object = json_array_get_object(json_array, i);
    jsonObject.subject = string(json_object_get_string(json_object, "subject"));
    jsonObject.location = string(json_object_get_string(json_object, "location"));
    
    tempObject = json_object;
        
    tempObject = (json_object_get_object(json_object, "start"));
    startTime = json_object_get_string(tempObject, "dateTime");

    jsonObject.start_timestamp = getGraphTimeAsTimestamp(startTime);

    json_object = (json_object_get_object(json_object, "end"));
    endTime = json_object_get_string(json_object, "dateTime");

    jsonObject.end_timestamp = getGraphTimeAsTimestamp(endTime);

    //DO POST METHOD 
    client.begin(wifi, "https://graph.microsoft.com/v1.0/users/marc.sahler@bachmann.info/calendar/getSchedule");

    client.addHeader("Authorization", ("Bearer " + jsonToken).c_str()); 
    client.addHeader("Content-Type", "application/json");
    client.addHeader("Prefer", "outlook.timezone=\"Europe/Berlin\"");  

    startTime = startTime.substr(0, startTime.size() - 8);
    endTime = endTime.substr(0, endTime.size() - 8);
    

    client.POST(("{\"schedules\": [\"" + jsonObject.subject.substr(0, jsonObject.subject.size() - 1) + "@bachmann.info\"], \"startTime\": { \"dateTime\": \"" + startTime +"\", \"timeZone\": \"Europe/Berlin\" },\"endTime\": { \"dateTime\": \"" + endTime +"\", \"timeZone\": \"Europe/Berlin\"  }, \"availabilityViewInterval\": 5}").c_str());
    
    json_DeserializeUser(client.getString().c_str(), jsonObject, startTime);
  }   
  
  json_value_free(root_value);
}

void loop() 
{

  getJson();
  


  v_Meetings.clear();
 
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
  
  


  calender_text(Raumname, Raumnummer, getTime());
  
  // deep sleep 
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}