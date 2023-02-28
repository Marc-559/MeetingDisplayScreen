#include <Alberto_time.hpp>
#include "time.h"
#include <WiFi.h>

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

//     time_t now;
//     struct tm timeinfo;
//     time(&now);
//     localtime_r(&now, &timeinfo);
//     // Is time set? If not, tm_year will be (1970 - 1900).
//     if (timeinfo.tm_year < (2016 - 1900)) {
//         printf("Time is not set yet. Connecting to WiFi and getting time over NTP.\n");
//         obtain_time();
//         // update 'now' variable with current time
//         time(&now);
//     }
// #ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
//     else {
//         // add 500 ms error to the current system time.
//         // Only to demonstrate a work of adjusting method!
//         {
//             printf(TAG, "Add a error for test adjtime\n");
//             struct timeval tv_now;
//             gettimeofday(&tv_now, NULL);
//             int64_t cpu_time = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
//             int64_t error_time = cpu_time + 500 * 1000L;
//             struct timeval tv_error = { .tv_sec = error_time / 1000000L, .tv_usec = error_time % 1000000L };
//             settimeofday(&tv_error, NULL);
//         }

//         printf(TAG, "Time was set, now just adjusting it. Use SMOOTH SYNC method.\n");
//         obtain_time();
//         // update 'now' variable with current time
//         time(&now);
}
//#endif

    // char strftime_buf[64];

    // // Set timezone to Eastern Standard Time and print local time
    // setenv("TZ", "CET-1CEST", 1); // List with all Timezones at: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    // tzset();
    // localtime_r(&now, &timeinfo);
    // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    // printf("The current date/time in New York is: %s\n", strftime_buf);

    

    // if (sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH) {
    //     struct timeval outdelta;
    //     while (sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS) {
    //         adjtime(NULL, &outdelta);
    //         printf( "Waiting for adjusting time ... outdelta = %li sec: %li ms: %li us\n",
    //                     (long)outdelta.tv_sec,
    //                     outdelta.tv_usec/1000,
    //                     outdelta.tv_usec%1000);
    //         vTaskDelay(2000 / portTICK_PERIOD_MS);
    //     }
    // }

    //const int deep_sleep_sec = 10;
    //printf(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
    //esp_deep_sleep(1000000LL * deep_sleep_sec);
//}


static void obtain_time(void)
{
    

    

    initialize_sntp();

    // // wait for time to be set
    // time_t now = 0;
    // struct tm timeinfo = { 0 };
    // int retry = 0;
    // const int retry_count = 10;
    // while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
    //     printf( "Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }
    // time(&now);
    // localtime_r(&now, &timeinfo);

    
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