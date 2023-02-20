#include <epaper.hpp>
#include<json_Deserialize.hpp>
#include <iomanip>
#include <ctime>
#include <sstream>

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include <U8g2_for_Adafruit_GFX.h>

// select the display class (only one), matching the kind of display panel
//#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
//#define GxEPD2_DISPLAY_CLASS GxEPD2_7C

// select the display driver class (only one) for your  panel
// 3-color e-papers
//#define GxEPD2_DRIVER_CLASS GxEPD2_154c     // GDEW0154Z04 200x200, no longer available
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_Z90c // GDEH0154Z90 200x200
//#define GxEPD2_DRIVER_CLASS GxEPD2_213c     // GDEW0213Z16 104x212
//#define GxEPD2_DRIVER_CLASS GxEPD2_290c     // GDEW029Z10  128x296
//#define GxEPD2_DRIVER_CLASS GxEPD2_270c     // GDEW027C44  176x264
//#define GxEPD2_DRIVER_CLASS GxEPD2_420c     // GDEW042Z15  400x300
//#define GxEPD2_DRIVER_CLASS GxEPD2_583c     // GDEW0583Z21 600x448
#define GxEPD2_DRIVER_CLASS GxEPD2_750c     // GDEW075Z09  600x384
//#define GxEPD2_DRIVER_CLASS GxEPD2_750c_Z08 // GDEW075Z08  800x480
//#define GxEPD2_DRIVER_CLASS GxEPD2_750c_Z90 // GDEH075Z90  880x528

// somehow there should be an easier way to do this
#define GxEPD2_BW_IS_GxEPD2_BW true
#define GxEPD2_3C_IS_GxEPD2_3C true
#define GxEPD2_7C_IS_GxEPD2_7C true
#define GxEPD2_1248_IS_GxEPD2_1248 true
#define IS_GxEPD(c, x) (c##x)
#define IS_GxEPD2_BW(x) IS_GxEPD(GxEPD2_BW_IS_, x)
#define IS_GxEPD2_3C(x) IS_GxEPD(GxEPD2_3C_IS_, x)
#define IS_GxEPD2_7C(x) IS_GxEPD(GxEPD2_7C_IS_, x)
#define IS_GxEPD2_1248(x) IS_GxEPD(GxEPD2_1248_IS_, x)

#if defined(ESP32)
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));
#endif

// alternately you can copy the constructor from GxEPD2_display_selection.h of GxEPD_Example to here
// and adapt it to the ESP32 Driver wiring, e.g.
//GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)); // GDEH0154D67

// comment out unused bitmaps to reduce code space used
//#include "bitmaps/Bitmaps200x200.h" // 1.54" b/w
//#include "bitmaps/Bitmaps104x212.h" // 2.13" b/w flexible GDEW0213I5F
//#include "bitmaps/Bitmaps128x250.h" // 2.13" b/w
//#include "bitmaps/Bitmaps128x296.h" // 2.9"  b/w
//#include "bitmaps/Bitmaps176x264.h" // 2.7"  b/w
//#include "bitmaps/Bitmaps400x300.h" // 4.2"  b/w
#include "bitmaps/Bitmaps640x384.h" // 7.5"  b/w
// 3-color
//#include "bitmaps/Bitmaps3c200x200.h" // 1.54" b/w/r
//#include "bitmaps/Bitmaps3c104x212.h" // 2.13" b/w/r
//#include "bitmaps/Bitmaps3c128x296.h" // 2.9"  b/w/r
//#include "bitmaps/Bitmaps3c176x264.h" // 2.7"  b/w/r
#include "bitmaps/Bitmaps3c400x300.h" // 4.2"  b/w/r

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
#define schrift_fuer_now u8g2_font_helvB18_te
#define schrift_fuer_past_and_next u8g2_font_helvB14_te

void epaper_setup()
{
  Serial.println();
  Serial.println("setup");
  display.init(115200); // uses standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
  // *** special handling for Waveshare ESP32 Driver board *** //
  // ********************************************************* //
  SPI.end(); // release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
  //SPI: void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
  SPI.begin(13, 12, 14, 15); // map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15)
  // *** end of special handling for Waveshare ESP32 Driver board *** //
  // **************************************************************** //
  // first update should be full refresh

  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX
  display.refresh();
  delay(1000);
  
  display.powerOff();
  ////deepSleepTest();
  Serial.println("setup done");
}

// note for partial update window and setPartialWindow() method:
// partial update window size and position is on byte boundary in physical x direction
// the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
// see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()

void calender_text(string raumname, string raumnummer, string date)
{
  u8g2Fonts.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);            // left to right (this is default)
  u8g2Fonts.setFont(u8g2_font_helvR14_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  
  display.setRotation(0);
  
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
  int16_t tbx, tby; 
  uint16_t tbw, tbh;
  tbw = u8g2Fonts.getUTF8Width(raumnummer.c_str());
  display.setFullWindow();
  
  display.firstPage();
  do
  {
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    
    // center bounding box by transposition of origin:
    uint16_t x = ((display.width() / 2) - tbw);
    //uint16_t y = ((display.height() - tbh) / 2) - tby;

    //Raum Angaben und Datum
    u8g2Fonts.setCursor(x,35);
    u8g2Fonts.setFont(u8g2_font_helvB24_tr);  
    u8g2Fonts.printf("%s",raumname.c_str());
      
    u8g2Fonts.setFont(u8g2_font_helvB18_tr);  
    display.getTextBounds(raumnummer.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
    
    u8g2Fonts.setCursor(0,tbh+25);
    u8g2Fonts.printf(raumnummer.c_str());

    /// Gelber Balken
    display.drawRect(0,40,display.width(),15,GxEPD_YELLOW);
    display.fillRect(0,40,display.width(),15,GxEPD_YELLOW);

    display.getTextBounds(date.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) - 58);
    uint16_t y = 25 + tbh;
    u8g2Fonts.setCursor(x,y);
    u8g2Fonts.printf(date.c_str());

    int meeting_now = 0;
    int meeting_next = 0;
    int meeting_past = -1;
    time_t timestamp = time(nullptr);
    
    // //errechnet den aktuellen Termin
    for (size_t i = 0; i < v_Meetings.size(); i++)
    {
      if((v_Meetings[i].start_timestamp <= timestamp) && ( v_Meetings[i].end_timestamp >= timestamp))
      {
        meeting_now = i;
        break;
      }
      else 
      {
        meeting_now = -1;  
      }
    }

    // // errechnet den nächsten Termin
    for (size_t j = 0; j < v_Meetings.size(); j++)
    {
      if(v_Meetings[j].start_timestamp >= timestamp)
      {
        meeting_next = j;
        break;
      }
      else
      {
        meeting_next = -1;
      }
    }
 
  	// //errechnet den letzten Termin
    for (size_t k = v_Meetings.size() -1; k >= 0; k--)
    {
      if(v_Meetings[k].end_timestamp <= timestamp)
      {
        meeting_past = k;
        
        break;
      }else{
        meeting_past = -1;
      }
    }
    
    if(meeting_past != -1)
    {
      u8g2Fonts.setFont(schrift_fuer_past_and_next);
      u8g2Fonts.setCursor(0,105);
      u8g2Fonts.setFont(schrift_fuer_past_and_next);
      u8g2Fonts.println(v_Meetings[meeting_past].subject.c_str());
      u8g2Fonts.setFont(schrift_fuer_past_and_next);
      u8g2Fonts.printf("%s - %s   %s", get_formated_time(v_Meetings[meeting_past].start_timestamp)
                      .c_str(),get_formated_time(v_Meetings[meeting_past].end_timestamp)
                      .c_str(), v_Meetings[meeting_past].organizer_name.c_str());
    }
    else{
      u8g2Fonts.setFont(schrift_fuer_past_and_next);
      u8g2Fonts.setCursor(0,105);
      u8g2Fonts.printf("-------");
    }
    
    if (meeting_now == -1)
    {
      u8g2Fonts.setCursor(0,200);
      u8g2Fonts.setFont(schrift_fuer_now);
      u8g2Fonts.printf("Free");
    }

    if(meeting_now != -1)
    {
      u8g2Fonts.setCursor(0,200);
      u8g2Fonts.setFont(schrift_fuer_now);
      u8g2Fonts.println(v_Meetings[meeting_now].subject.c_str());
      u8g2Fonts.printf("%s - %s   %s", get_formated_time(v_Meetings[meeting_now]
                      .start_timestamp).c_str(),get_formated_time(v_Meetings[meeting_now].end_timestamp)
                      .c_str(), v_Meetings[meeting_now].organizer_name.c_str());
      u8g2Fonts.setCursor(0,200);
    }

  	if(meeting_next != -1)
    {
      u8g2Fonts.setCursor(0,310);
      u8g2Fonts.setFont(schrift_fuer_past_and_next);
      u8g2Fonts.println(v_Meetings[meeting_next].subject.c_str());
      u8g2Fonts.printf("%s - %s   %s", get_formated_time(v_Meetings[meeting_next].start_timestamp)
                      .c_str(),get_formated_time(v_Meetings[meeting_next]
                      .end_timestamp).c_str(), v_Meetings[meeting_next].organizer_name.c_str());
    }
    else
    {
      cout << "Test 8" << endl;
      u8g2Fonts.setFont(schrift_fuer_past_and_next);
      u8g2Fonts.setCursor(0,310);
      u8g2Fonts.printf("-------");
    }
  }
  while (display.nextPage());
  
  display.powerOff();
}

//Funktion um Time Stamp in zeit im Format: 11:00 - 11:15 zu konvertieren 
 
string get_formated_time(time_t timestamp)
{
    auto tm = *std::localtime(&timestamp);
    std::ostringstream s; 
    s << put_time(&tm, "%R");
    string time = s.str();

    return time;
}