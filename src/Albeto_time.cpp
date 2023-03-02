#include <Alberto_time.hpp>
#include "time.h"
#include <WiFi.h>
#include <iostream>
#include "Alberto_mqtt.hpp"
#include <iomanip>
#include <sstream>





static void obtain_time(void);
static void initialize_sntp(void);

const char* ntpServer1 = "10.10.10.32";
const char* ntpServer2 = "217.13.75.19";
const char* ntpServer3 = "217.13.75.16";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_CUSTOM
void sntp_sync_time(struct timeval *tv)
{
   settimeofday(tv, NULL);
   printf(TAG, "Time is synchronized from custom code\n");
   sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
}
#endif

void time_sync_notification_cb(struct timeval *tv)
{
    printf("Notification of a time synchronization event\n");
}

  
void time_main(void)
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
}


static void obtain_time(void)
{
    initialize_sntp();
}

static void initialize_sntp(void)
{
    printf("Initializing SNTP\n");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "91.206.8.34");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}

string getTime()
{
    time_t t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream s; 
  s << put_time(&tm, "%Y-%m-%d");
  string date = s.str();
  return date;
}
