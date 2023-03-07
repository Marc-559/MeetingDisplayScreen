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

vector<Meeting_struct> v_Meetings;

static HTTPClient client;

time_t getGraphTimeAsTimestamp(String datetime) 
{
  tm tm;   
  strptime(datetime.c_str(), "%Y-%m-%dT%H:%M:%S.%f", &tm);

  return mktime(&tm) + 60 * 60;
}

 void json_DeserializeUser(string payload, Meeting_struct jsonMeetingValue)
{ 
  JSON_Value *root_value;
  JSON_Array *json_array;
  JSON_Object *json_object;

  root_value = json_parse_string(payload.c_str());
  
  //std::cout << "PAYLOAD USER: " << payload.c_str() << "\n";

  json_object = json_value_get_object(root_value); // json object = full json
  json_array = json_object_get_array(json_object, "value"); //json array = value array

  json_object = json_array_get_object(json_array, 0); //json object = value array (value) 
  json_array = json_object_get_array(json_object, "scheduleItems"); // json array = scheduleItemsArray in Value array

  for (size_t i = 0; i < json_array_get_count(json_array); i++) // für jedes scheduleitems array in value array
  {
    json_object = json_array_get_object(json_array, i);

    if (!string(json_object_get_string(json_object, "location")).compare( jsonMeetingValue.location))
    {   
      jsonMeetingValue.subject = string(json_object_get_string(json_object, "subject"));          
        std::cout << "jsonData -> subject : " << jsonMeetingValue.subject << "\n";
        std::cout << "jsonData -> location : " << jsonMeetingValue.location << "\n";

    }          
  }
  json_value_free(root_value);
}



void doPost(Meeting_struct jsonMeetingValue, string token, WiFiClientSecure wifi, string startTime, string endTime)
{
  
  


  client.begin(wifi, "https://graph.microsoft.com/v1.0/users/marc.sahler@bachmann.info/calendar/getSchedule");

  client.addHeader("Authorization", ("Bearer " + token).c_str()); 
  client.addHeader("Content-Type", "application/json");
  client.addHeader("Prefer", "outlook.timezone=\"Europe/Berlin\"");  

  startTime = startTime.substr(0, startTime.size() - 8);
  endTime = endTime.substr(0, endTime.size() - 8);
  jsonMeetingValue.subject = jsonMeetingValue.subject.substr(0, jsonMeetingValue.subject.size() - 1);

  std::cout << "NAME" << jsonMeetingValue.subject << "\n";
  std::cout << "STARTTIME" << startTime <<"\n";
  std::cout << "ENDTIME" << endTime <<"\n";
  std::cout << "CONNECTION STATUS" << client.connected() <<"\n";
  
  std::cout << "Before POST" << "\n";


  client.POST(("{\"schedules\": [\"" + jsonMeetingValue.subject + "@bachmann.info\"], \"startTime\": { \"dateTime\": \"" + startTime +"\", \"timeZone\": \"Europe/Berlin\" },\"endTime\": { \"dateTime\": \"" + endTime +"\", \"timeZone\": \"Europe/Berlin\"  }, \"availabilityViewInterval\": 5}").c_str());
  std::cout << "AFTER POST" << "\n";    

  json_DeserializeUser(client.getString().c_str(), jsonMeetingValue);
  client.end(); 
  


}

void json_DeserializeMeetingRoom(string payload, string token, WiFiClientSecure wifi)
{
  JSON_Value *root_value;
  JSON_Array *json_array;
  JSON_Object *json_object;
  Meeting_struct jsonMeetingValue;
  String startTime;
  String endTime;

  root_value = json_parse_string(payload.c_str());
  json_object = json_value_get_object(root_value);// json object = full json
  json_array = json_object_get_array(json_object, "value"); //json array = value array


  json_object = json_array_get_object(json_array, 0); //json object = value array (value) 
  json_array = json_object_get_array(json_object, "scheduleItems"); // json array = scheduleItemsArray in Value array

  JSON_Object *tempObject;
  client.setReuse(true);
  for (size_t i = 0; i < json_array_get_count(json_array); i++) // für jedes scheduleitems array in value array
  {
    json_object = json_array_get_object(json_array, i);
    jsonMeetingValue.subject = string(json_object_get_string(json_object, "subject"));
    jsonMeetingValue.location = string(json_object_get_string(json_object, "location"));
        
    tempObject = json_object;
      
    tempObject = (json_object_get_object(json_object, "start"));
    startTime = json_object_get_string(tempObject, "dateTime");

    jsonMeetingValue.start_timestamp = getGraphTimeAsTimestamp(startTime);

    json_object = (json_object_get_object(json_object, "end"));
    endTime = json_object_get_string(json_object, "dateTime");

    jsonMeetingValue.end_timestamp = getGraphTimeAsTimestamp(endTime);

    if (i == 2) {
      client.begin(wifi, "https://www.google.com");
      client.GET();  
      client.end();
    }
    std::cout << "BEFORE DOPOST METHOD" << "\n";
    doPost(jsonMeetingValue, token, wifi, startTime.c_str(), endTime.c_str());

    v_Meetings.emplace_back(jsonMeetingValue);
  }   
  
  json_value_free(root_value);

}
  
