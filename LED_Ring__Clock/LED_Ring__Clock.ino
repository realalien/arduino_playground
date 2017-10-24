// Credits
//REF: Init code, https://www.hackster.io/ingo-lohs/mylight-clock-with-neopixel-ring-12-controlled-by-photon-59ce46
//REF: RTC R/W, https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/wiring-and-test
//REF: SD card, http://educ8s.com/Arduino/SDCard/SDCard_demo.ino
//Local: PIR_test sketch

// Purpose:
// * To provide illumination for room without switching the normal lights which is too much during mid-night.


// Wiring:
// <link>


// IDEAS & TODOs:
// * Q: How to make the code strong(still working LEDs) even some parts are removed one by one? 
//   A:  


#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_NeoPixel.h>
#include <SD.h>
#include <SPI.h>

const int MOTION_PIN = 3; // Pin connected to motion detector
const int LED_PIN = LED_BUILTIN; // LED pin - active-high
const int CS_PIN = 10;

RTC_DS3231 rtc;
File file;

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_COUNT 12
#define PIXEL_PIN 6
#define PIXEL_TYPE WS2812


//int brightness_h = 10;        // 0-256 nonbright to bright
//int brightness_m = 10;        // 0-256 nonbright to bright
int delayval = 1000;          // update scene
int min_px;                   // minute-pixel
int h,m;                        // hour, miniute
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
bool showPixelFlat = false;
bool lastStatus = false;
int auto_timeout = 90;  // SUG: should change to save power by learning behaviors(recorded history) Find pattern in day, week, month?
char *file2w;
DateTime lastInvokeTimestamp;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB+ NEO_KHZ800);  // Instance the object

void setup() {
  Serial.begin(9600);

  // PIR
  // The PIR sensor's output signal is an open-collector, 
  // so a pull-up resistor is required:
  pinMode(MOTION_PIN, INPUT_PULLUP);

  
  // RTC
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
  strip.clear();
  strip.show();

  // Timer , do NOT use millis() in case of overflow
  lastInvokeTimestamp = rtc.now();

  // Peristent on file
  initializeSD();
  file2w="sen_data/PIRCLK_1.TXT";
  createDirAtRoot("sen_data");
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

  int proximity = digitalRead(MOTION_PIN);

  if (proximity == HIGH) {
    // If the sensor's output goes HIGH, motion is detected
    digitalWrite(LED_PIN, HIGH);
    showPixelFlat = true;
    //reset timestamp 
    lastInvokeTimestamp = rtc.now();
  } else {
    digitalWrite(LED_PIN, LOW);
    // set showPixelFlat to false if timeout
    if ( (rtc.now() - lastInvokeTimestamp).totalseconds() > auto_timeout && showPixelFlat ) {
      Serial.println("Turning off....");
      showPixelFlat = false;
      strip.clear();
      strip.show();
    }
  }

  // show piexel first, then record
  if (showPixelFlat) {
    showNeoPixel_12();
  }else {
    strip.clear();
    strip.show();
  }

  // use lastStatus instead of proximity to avoid multiple proximity invokes.
  log_status_change(showPixelFlat);
  
  delay(delayval);
  
}

/*****************************************************************  . 
*  SD card related
******************************************************************/
void initializeSD()
{
  pinMode(CS_PIN, OUTPUT);

  if (SD.begin())
  {
//    Serial.println("SD card is ready to use.");
  } else
  {
//    Serial.println("SD card initialization failed");
    return;
  }
}

void log_status_change(bool isOn){

  if (isOn != lastStatus ) {
//     Serial.print("log_status_change .... new status : ");
//     Serial.println(isOn ? "ON" : "OFF");
     char buffer[50];
     
      // TODO: decoulple the RTC with File peristence
      DateTime dt = rtc.now();
      sprintf(buffer, "%d-%02d-%02dT%02d:%02d:%02d\t%s\0", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second(), isOn ? "ON":"OFF");
//      Serial.println(buffer);
      write_recording(buffer);
      // change to new status
      lastStatus = isOn;
      delay(2000);
  }
}

void write_recording(char text[]){
  createFile(file2w);
  writeToFile(text);
  closeFile();
}

void createDirAtRoot(char *folderName){
    if (SD.exists(folderName)) {
      Serial.println("Folder exists");
      //      printDirectory(entry, numTabs + 1);
    } else {
      SD.mkdir(folderName);
    }
}

int createFile(char filename[])
{
  file = SD.open(filename, FILE_WRITE);

  if (file)
  {
//    Serial.println("File created successfully.");
    return 1;
  } else
  {
//    Serial.println("Error while creating file.");
    return 0;
  }
}

int writeToFile(char text[])
{
  if (file)
  {
    file.println(text);
    Serial.print("Writing to file: ");
    Serial.println(text);
    return 1;
  } else
  {
    Serial.println("Couldn't write to file");
    return 0;
  }
}

void closeFile()
{
  if (file)
  {
    file.close();
//    Serial.println("File closed");
  }
}

int openFile(char filename[])
{
  file = SD.open(filename);
  if (file)
  {
//    Serial.println("File opened with success!");
    return 1;
  } else
  {
    Serial.println("Error opening file...");
    return 0;
  }
}

String readLine()
{
  String received = "";
  char ch;
  while (file.available())
  {
    ch = file.read();
    if (ch == '\n')
    {
      return String(received);
    }
    else
    {
      received += ch;
    }
  }
  return "";
}

/*****************************************************************  . 
*  NeoPixel related
******************************************************************/
void showNeoPixel_12(){
    DateTime now = rtc.now();

    h = now.hour() % 12 ;
    m = now.minute();
    
    strip.clear(); // all already switched pixel turned off

    color_scheme();
    
    if (h == min_px) {
      strip.setPixelColor((h+6)%12,R_equal, G_equal, B_equal); // h: 1 Pixel in Yellow if hour and minute-range is equal       
      strip.show();
    } else {
      // "(hr_or_min + 6) % 12" to allow correct display in up-side down position 
      strip.setPixelColor((h+6)%12,R_h, G_h, B_h); // h: 1 Pixel in Red
      strip.setPixelColor((min_px+6)%12, R_m, G_m, B_m); // min_px: 1 Pixel represents a period from 5 min  
      strip.show();
    }
  }

void color_scheme(){
//    // Scheme 1
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
    //min_px = m / 5;
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
}

