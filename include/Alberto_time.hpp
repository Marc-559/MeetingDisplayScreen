#ifndef __h_alberto_time
#define __h_alberto_time

using namespace std;

#include <string>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "esp_sntp.h"


extern time_t now;
extern struct tm timeinfo;

void time_main(void);
string getTime(void);
string getTimejson(void);

#endif