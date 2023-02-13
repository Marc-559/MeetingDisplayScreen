#include "json_Deserialize.hpp"

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <epaper.hpp>

vector<Meeting_struct> v_Meetings;

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
        
        
    }
   
}

