#include "json_Deserialize.hpp"

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <epaper.hpp>

vector<Meeting_struct> v_Meetings;


void json_DeserializeMeetingRoom(string payload)
{
    JSON_Value *root_value;
    JSON_Array *json_array;
    JSON_Object *json_object;
    Meeting_struct jsonMeetingValue;
    
    root_value = json_parse_string(payload.c_str());
    json_object = json_value_get_object(root_value); // json object = full json
    json_array = json_object_get_array(json_object, "value"); //json array = value array


    for (size_t i = 0; i < json_array_get_count(json_array); i++) // für jedes value array 
    {
      json_object = json_array_get_object(json_array, i); //json object = value array (value) 
      json_array = json_object_get_array(json_object, "scheduleItems"); // json array = scheduleItemsArray in Value array

      for (size_t i = 0; i < json_array_get_count(json_array); i++) // für jedes scheduleitems array in value array
      {
        json_object = json_array_get_object(json_array, i);
        jsonMeetingValue.subject = string(json_object_get_string(json_object, "subject"));

        //v_Meetings.emplace_back(weather);
        std::cout << "#############jsonData############ : " << jsonMeetingValue.subject << "\n";
       

      }
    }

}

void json_DeserializeUser(string payload)
{
    JSON_Value *root_value;
    JSON_Array *json_array;
    JSON_Object *json_object;
    Meeting_struct jsonUserValue;
    
    root_value = json_parse_string(payload.c_str());
    json_object = json_value_get_object(root_value); // json object = full json
    json_array = json_object_get_array(json_object, "value"); //json array = value array


    for (size_t i = 0; i < json_array_get_count(json_array); i++) // für jedes value array 
    {
      json_object = json_array_get_object(json_array, i); //json object = value array (value) 
      json_array = json_object_get_array(json_object, "scheduleItems"); // json array = scheduleItemsArray in Value array

      for (size_t i = 0; i < json_array_get_count(json_array); i++) // für jedes scheduleitems array in value array
      {
        json_object = json_array_get_object(json_array, i);
        jsonUserValue.subject = string(json_object_get_string(json_object, "subject"));

        std::cout << "#############jsonData############ : " << jsonUserValue.subject << "\n";
      }
    }
}

 void json_Deserialize(string payload)
  {
     JSON_Value *root_value;
     JSON_Array *meeting_array;
     JSON_Object *meeting_object;
     size_t i;

    root_value = json_parse_string(payload.c_str());
     if (json_value_get_type(root_value) != JSONArray) {
        
     }
    
     meeting_array = json_value_get_array(root_value);
    Meeting_struct meeting ;
    for (i = 0; i < json_array_get_count(meeting_array); i++) {
        meeting_object = json_array_get_object(meeting_array, i);
        meeting.subject = string(json_object_get_string(meeting_object, "subject"));
        meeting.organizer_name =  string(json_object_get_string(meeting_object, "organizer_name")); 
        meeting.sensitivity = string(json_object_get_string(meeting_object, "sensitivity"));
        meeting.start_timestamp = json_object_get_number(meeting_object, "start_stamp");
        meeting.end_timestamp = json_object_get_number(meeting_object, "end_stamp");
        v_Meetings.emplace_back(meeting);
        
        std::cout << meeting.subject << "\n" ; 
    }
   
}

