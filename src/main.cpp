#include <Arduino.h>
#include <WiFi.h>
#include <stdbool.h>
#include <vector>
#include "Alberto_mqtt.hpp"
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

using namespace std;

const char *ssid = "H@cka1r0n"; // "your ssid";
const char *password = "!H@cka1h0n!";   // "your password";

// deep sleep defines
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60 // 1 minute(n)        /* Time ESP32 will go to sleep (in seconds) */

// Raum defines
#define Raumname "Hawaii"
#define Raumnummer "D2/16"

// IP address in your local wifi net
IPAddress myIP;        
vector <string> strings;
Meeting_struct x;

void setup() {

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
  

  // mqtt_setup();
  // mqtt_connect();

  epaper_setup();
  time_main();

}

// Is getting Json from attached URL
void getJson() 
{
   vector<Meeting_struct> v_Token;
   Meeting_struct token;

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

    WiFiClientSecure wifi;
    const char* address = "login.micrososftonline.com";
    int port = 443;
    HTTPClient client;
    client.begin(wifi, "https://login.microsoftonline.com/e3bace4d-d2e7-4d8f-afb8-152509ee3f1a/oauth2/v2.0/token");
    client.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    wifi.setCACert(fingerprint);

    wifi.setInsecure();

    String contentType = "application/x-www-form-urlencoded";
    String postData = "client_id=e29d3f70-8b7b-43a9-a2a2-ce5f0d7fdfef&scope=https%3A%2F%2Fgraph.microsoft.com%2F.default&client_secret=2S78Q~a4Kq~DswvIVfk6i_m3BvDL4cRWh1qq3deN&grant_type=client_credentials";

    //std::cout << ">>>>>>>>>>>>>>>>: " << client.get("/common/oauth2/v2.0/token") << "\n";

    int statusCode = client.POST(postData);
    String jsonObject = client.getString();

   
    //std::cout << "statusCode: " << statusCode << "\n";

    string tempString = jsonObject.substring(79).c_str();
    string jsonToken = tempString.substr(0, tempString.size() - 2);
  //std::cout << "<<<<<<<<<TOKEN>>>>>>>>>>" << "\n\n";
  //std::cout << jsonToken << "\n";
   
  client.begin(wifi, "https://graph.microsoft.com/v1.0/users/marc.sahler@bachmann.info/calendar/getSchedule");


    client.addHeader("Authorization", ("Bearer " + jsonToken).c_str());
    client.addHeader("Content-Type", "application/json");
    client.addHeader("Prefer", "outlook.timezone=\"Europe/Berlin\"");
    int statuscode = client.POST(("{\"schedules\": [\"_bzd2-16.hawaii@bachmann.info\"], \"startTime\": { \"dateTime\": \"" + getTime() + "T07:30:00\", \"timeZone\": \"Europe/Berlin\" },\"endTime\": { \"dateTime\": \"" + getTime() + "T20:00:00\", \"timeZone\": \"Europe/Berlin\"  }, \"availabilityViewInterval\": 60}").c_str());
    String requestValue = client.getString();
    
    json_DeserializeMeetingRoom(requestValue.c_str());
  
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
  
  

  calender_text(Raumname, Raumnummer, getTime());
  
  // deep sleep 
  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // esp_deep_sleep_start();
}


