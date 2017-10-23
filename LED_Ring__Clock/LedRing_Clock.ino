


// Credits
//REF: Init code, https://www.hackster.io/ingo-lohs/mylight-clock-with-neopixel-ring-12-controlled-by-photon-59ce46
//REF: RTC R/W, https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/wiring-and-test

#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_NeoPixel.h>

RTC_DS3231 rtc;

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_COUNT 12
#define PIXEL_PIN 6
#define PIXEL_TYPE WS2812

int brightness_h = 10;        // 0-256 nonbright to bright
int brightness_m = 10;        // 0-256 nonbright to bright
int delayval = 1000;          // update scene
int min_px;                   // minute-pixel
int h;                        // hour
unsigned color_m;             // minute-color
int R_h;                      // color-Definitions for hour and minute in RGB and if hour and minute-range is equal
int G_h;
int B_h;
int R_m;
int G_m;
int B_m;
int R_equal = 128;
int G_equal = 128;            // yellow
int B_equal = 0;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB+ NEO_KHZ800);  // Instance the object

void setup() {
  Serial.begin(9600);

  // RTC
  //Time.zone (+2.00); // setup a timezone > in this case Berlin
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Neopixel
  strip.begin();
}

   /* the colors
    * yellow - 128,128,0
    * orange - 255,128,0
    * red    - 255,0,0
    * green  - 0,255,0
    * blue   - 0,0,255
    * purple - 128,0,128
    */

void loop() {
    
//    if (Time.hourFormat12() == 12) {  // neopixel not count from 1-12 like the time-function. we need to change 12 o´clock to pixel 0
//    h = 0;
//    } else {
//    h = Time.hourFormat12(); // https://docs.particle.io/reference/firmware/photon/#hourformat12- - returns integer 1-12
//    }

    DateTime now = rtc.now();

    h = now.hour() % 12 ;
//    time_t Time = now();
//    if (now.hourFormat12() == 12) {  // neopixel not count from 1-12 like the time-function. we need to change 12 o´clock to pixel 0
//    h = ;
//    } else {
//    h = Time.hourFormat12(); // https://docs.particle.io/reference/firmware/photon/#hourformat12- - returns integer 1-12
//    }

    Serial.println("**********VALIDATION HOUR******************************");
    Serial.print("Device-Function: ");
//    Serial.println(Time.hourFormat12());
    Serial.print("Hour: ");
    Serial.println(h);

    int m = now.minute();
    
    strip.clear(); // all already switched pixel turned off

//    if (m<5) {
//      min_px = 0;                   // 0 = first of in this case 12 pixel
//      color_m = (0,0,255);          // blue
//    } else if (m >= 5 && m<10) {
//      min_px = 1;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 10 && m<15) {
//      min_px = 2;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 15 && m<20) {
//      min_px = 3;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 20 && m<25) {   
//      min_px = 4;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 25 && m<30) {
//      min_px = 5;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 30 && m<35) {
//      min_px = 6;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 35 && m<40) {
//      min_px = 7;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 40 && m<45) {
//      min_px = 8;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 45 && m<50) {
//      min_px = 9;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 50 && m<55) {
//      min_px = 10;        
//      color_m = (0,0,255);          // blue
//    } else if (m >= 55) {
//      min_px = 11;        
//      color_m = (0,0,255);          // blue
//    }


    // Color Scheme 2, https://github.com/ILohs/MySketches/blob/master/MyNeopixelClock_v1.1
    if (m<5) {
      min_px = 0;  // 0 = first of in this case 12 pixel
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 5 && m<10) {
      min_px = 1;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 10 && m<15) {
      min_px = 2;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 15 && m<20) {
      min_px = 3;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 20 && m<25) {   
      min_px = 4;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 25 && m<30) {
      min_px = 5;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 30 && m<35) {
      min_px = 6;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 35 && m<40) {
      min_px = 7;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 40 && m<45) {
      min_px = 8;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 45 && m<50) {
      min_px = 9;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 50 && m<55) {
      min_px = 10;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    } else if (m >= 55) {
      min_px = 11;        
        R_m = 0;
        G_m = 0;   // blue
        B_m = 255;
    }
    
    /* the colors
    * yellow - 128,128,0
    * orange - 255,128,0
    * red    - 255,0,0
    * green  - 0,255,0
    * blue   - 0,0,255
    * purple - 128,0,128
    */
    
    if (h==0) {
        R_h = 255;
        G_h = 0;   // red at 12
        B_h = 0;
    } else if (h==1) {
        R_h = 255;
        G_h = 128; // orange at 1
        B_h = 0;
    } else if (h==2) {
        R_h = 128;
        G_h = 0;   // purple at 2 
        B_h = 128;
    } else if (h==3) {
        R_h = 255;
        G_h = 0;   // red at 3
        B_h = 0;
    } else if (h==4) {   
        R_h = 128;
        G_h = 0;   // purple at 4
        B_h = 128;
    } else if (h==5) {
        R_h = 255;
        G_h = 128;   // orange at 5
        B_h = 0;
    } else if (h==6) {
        R_h = 255;
        G_h = 0;   // red at 6
        B_h = 0;
    } else if (h==7) {
        R_h = 255;
        G_h = 128; // orange at 7
        B_h = 0;
    } else if (h==8) {
        R_h = 128;
        G_h = 0;   // purple at 8
        B_h = 128;
    } else if (h==9) {
        R_h = 255;
        G_h = 0;   // red at 9
        B_h = 0;
    } else if (h==10) {
        R_h = 128;
        G_h = 0;   // purple at 10
        B_h = 128;
    } else if (h==11) {
        R_h = 255;
        G_h = 128;   // orange at 11
        B_h = 0;
    }
    
    if (h == min_px) {
      Serial.println("***********EXCEPTION CASE: HOUR = Range of Minute-Pixel");
      Serial.print("Hour: ");
      Serial.println(h);
      Serial.print("Minute: ");
      Serial.println(m);
      Serial.print("Minute-Range: ");
      Serial.println(min_px);
      Serial.println("hour and minute-range is equal - you see 1 pixel bright");

      strip.setPixelColor((h+6)%12,R_equal, G_equal, B_equal); // h: 1 Pixel in Yellow if hour and minute-range is equal      
      strip.setBrightness(brightness_h);    
      strip.show();
    } else {
      Serial.println("**********NORMAL CASE: HOUR <> Range of Minute-Pixel***");
      Serial.print("Hour: ");
      Serial.println(h);
      Serial.print("Minute: ");
      Serial.println(m);
      Serial.print("Minute-Range: ");
      Serial.println(min_px);
      Serial.println("hour and minute-range is NOT equal - you see 2 pixels bright");

      strip.setPixelColor((h+6)%12,R_h, G_h, B_h); // h: 1 Pixel in Red
      strip.setPixelColor((min_px+6)%12, R_m, G_m, B_m); // min_px: 1 Pixel represents a period from 5 min
      strip.show();
    }
      delay(delayval);
}
