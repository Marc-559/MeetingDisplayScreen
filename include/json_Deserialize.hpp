#include <vector>
#include "parson.h"
#include <MQTT.h>
#include <string>
#include <string.h>
#include <cstring>
#include <HTTPClient.h>

using namespace std;


struct Meeting_struct
{
    string subject;
    string organizer_name;
    string sensitivity;
    string location;  
    long start_timestamp;
    long end_timestamp;
};

extern vector<Meeting_struct> v_Meetings;
//void JsonSetup(const char **ssid, const char **password, string Raumname, string Raumnummer,string roomEmail);

