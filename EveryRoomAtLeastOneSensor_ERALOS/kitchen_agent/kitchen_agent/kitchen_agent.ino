#include <ArduinoJson.h>         // Install this library from Sketch->Include Library->Libray Manager
#include <ESP8266WiFi.h>
#include <dht.h>


//#define dht_apin A0
#define dht_apin 4
/**
 To send temperature and himidity to the local server

 Functionalities:
 > report data via WIFI
 > allow normal speed, burst speed for data collecting

 Future Usages:
 > compare with outdoor temperatue to study the changing temperature and humidity
 
**/


/**
 TODOs:
 * error handling and signaling via led if network problem or something
 * reconnect after WIFI lose
 * write to local storage if offline, bundle update after once get online
 * power saving with just battery for longer time
 * put it on VPS and with authroization/authentication
**/


const char* THIG_NAME = "AlienESP";  // Put your thing name here
const char* key = "Servo1";               // Put your key name here

const char* WIFISSID = "District9";     // Put your WiFi SSID here
const char* PASSWORD = "abcdefgh";    // Put your WiFi password here

int servoPin = 9;                // This is the pin to which your servo is attached to

const char* host = "192.168.2.99";
String ans;
String response;
uint8_t bodyPosinit;
uint8_t bodyPosend;
String dweetKey ="\"";
int length = dweetKey.length();
int default_interval_in_milli = 0; 

 
dht DHT;



void setup() {
    
    
    Serial.begin(115200);
    Serial.println("Sensor initiated");
    default_interval_in_milli = 15 * 1000;
    online();

    // TODO: check if target sensor is connected or not! Otherwise the chip will spill reset error

    delay(1000);  // Q: how long to wait for sensor? A:
    
}

void loop() {


      
//      connect();
      postData();


//    receiveDweet();
//    printValue();
//    Serial.println(ans);
//    Serial.println("answer: ===================");
//    
//    if(ans=="True" || ans=="true")
//    {
//      Serial.println("Flag On!");
//      mailFlag.write(90);
//      sendDweet();
//    }
}

void online(){
  Serial.print("Connecting to : ");
  Serial.println(WIFISSID);
  WiFi.begin(WIFISSID,PASSWORD);
  while(WiFi.status()!= WL_CONNECTED)
  {
      delay(100);
      Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());
}



void connect(){
  
  
}


void postData()
{
    
    Serial.print(">>>>>>>>>>  Posting data  ...");
    

    // TODO: can you make code into subroutine?
    WiFiClient client;
    const int httpPort = 4567;
    Serial.print("Connecting to : ");
    Serial.println(host);
    if (!client.connect(host, httpPort)) {
      Serial.println("Connection Failed");
      return;
    } 

    
    Serial.println("Connected Successfully!");
    Serial.println("Sending data! ");

    // TODO: what if network takes too long for sending the data? 
    
    // reading sensor and send 
    DHT.read11(dht_apin);
    delay(200);
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");

    
    
//    String postdata = "temp=";
//    postdata.concat(String(DHT.humidity));
//    postdata.concat("&humid=");
//    postdata.concat(String(DHT.temperature));  // in celsius

    // create an object
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& jsonData = jsonBuffer.createObject();
    jsonData["value"] = DHT.temperature;
    jsonData["sid"] = 1;
    
    // TODO: /temperature is not a good API interface to accept multiple value in one request
    //IDEA: put data in the form for privacy; also, 
    //IDEA: multiple readings of several sensor should be separate observation(record entry)
    client.println(String("POST /api/v1/temperature HTTP/1.1"));  // TODO: what if there are multiple kitchen rooms?
    client.println("Host: 192.168.2.99:4567");
    client.println("Cache-Control: no-cache");
    client.println("Accept: application/json");
    client.println("Content-Type: application/json; charset=UTF-8");
    client.print("Content-Length: ");
    client.println(jsonData.measureLength());
    client.println();
    jsonData.printTo(client);

    jsonData.prettyPrintTo(Serial);
//    client.println(jsonData);

    // TODO: more accurate interval despite other delay or sleep or network time spend
    // for now and for simplicity, the network should take less then 1 second, which is not significant

    
    long interval = 2000;
    unsigned long currentMillis = millis(), previousMillis = millis();


    Serial.print("1111111111111");
    while(!client.available()){
      if( (currentMillis - previousMillis) > interval ){
        Serial.println("Timeout");

        // TODO: blink the wifi led or something for signaling
        
//        blinkLed.detach();
//        digitalWrite(2, LOW);

        Serial.println("\nClosing Connection");
        client.stop();     
        return;
      }
      currentMillis = millis();
    }

    Serial.print("2222222222");
    while (client.connected())
    {
      if ( client.available() )
      {
        response = client.readStringUntil('\n');
        Serial.println(response);

       // TODO: parse out response here
       
      }      
    }

    Serial.print("333333333");
    client.stop();
    Serial.println("\nClosing Connection after response!");

    Serial.print("Recollect after seconds : ");
    Serial.println(String(default_interval_in_milli / 1000));

    //TODO: should not delay if anything wrong
    delay(default_interval_in_milli);
}



void receiveDweet()
{
    Serial.print("Receiving Data for ");
    Serial.println(THIG_NAME);
    WiFiClient client;
    const int httpPort = 80;
    Serial.print("Connecting to : ");
    Serial.println(host);
    if (!client.connect(host, httpPort)) {
      Serial.println("Connection Failed");
      return;
    }
    Serial.println("Connected Successfully!");
    client.print(String("GET /get/latest/dweet/for/"));
    client.print(THIG_NAME);
    client.print(String(" HTTP/1.1\r\nHost: dweet.io \r\n Connection: close\r\n\r\n"));
    delay(100);

    while (client.connected() ){
      if (client.available())
      {
        response = client.readStringUntil('\n');
        Serial.println(response);
      } 
    }
    client.stop();
    Serial.println("\nClosing Connection");
}

void printValue()
{
  
    Serial.println(response);
    Serial.println("Parsing response");
    bodyPosinit =4+ response.indexOf("\r\n\r\n");
    response = response.substring(bodyPosinit);
    Serial.println("Eliminating non-body part");
    Serial.println(response);

    Serial.println("Detecting content...");
    bodyPosinit =10+ response.indexOf("\"content\"");
    bodyPosend = response.indexOf("}]}");
    response = response.substring(bodyPosinit,bodyPosend);
    Serial.println(response);

    if (response.indexOf(dweetKey) == -1) {
      ans = "Key not found";
      delay(5 * 1000);
    } else {
      Serial.println("Parsing json...");
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(response);
        if (!root.success()){
          Serial.println("parseObject() failed");
          ans = "parse error.";
        } else {
          const char* val  = root[key];
          ans = String(val);
        }
     }
}

