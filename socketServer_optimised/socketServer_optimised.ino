#include <Arduino.h>
#include <Wire.h>
#include <LSM303.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Preferences.h> //get previous state from spiffs and possibly wifi creds

const char* ssid = "WR-32-srv"; //Enter SSID
const char* password = "50N6-99s"; //Enter Password
//String devicename = "TremorLabs Monitor V2"; //what we want the device to "look" like
long readings[30]; //sensor readings batch, to try and limit the messages per second. might not actually help...
int ptr = 0; //points to current position in readings

bool streamS = 0; //temporary stream enabled. not on SPIFFS
bool logS = 0; //if logging is enabled, no wifi and only n readings get logged to sd card. stored on SPIFFS

unsigned long millisWS;
unsigned long millisAC;
unsigned long currentMillis;
const unsigned long connwait = 200;
const unsigned long freqwait = 20;

LSM303 compass;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

Preferences preferences;

void initSDCard() {
  if(!SD.begin(27)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void handleMsg(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "start stream")==0) { // strcmp is a bit strange: it is not 1 if they are the same and 0 if different, but 0 if the same and !0 if longer / shorter (check https://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm)
      streamS = 1;
      Serial.println("started stream");
    } else if (strcmp((char*)data, "stop stream")==0) {
      streamS = 0;
      Serial.println("stopped stream");
    } else if (strcmp((char*)data, "start log")==0) {
      logS = 1;
      Serial.println("started log");
    } else if (strcmp((char*)data, "stop log")==0) {
      logS = 0;
      Serial.println("stopped log");
    } else {
      Serial.println((char*)data);
    }
  }
}

void wsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  switch(type) {
    case WS_EVT_CONNECT:
      Serial.println("Connection received");
      break;
    case WS_EVT_DISCONNECT:
      Serial.println("Connection terminated");
      break;
    case WS_EVT_DATA:
      handleMsg(arg, data, len);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  preferences.begin("settings", false);
  streamS = preferences.getBool("stream", true);
  logS = preferences.getBool("log", false);
  preferences.end();
  Serial.println("Settings:")
  Serial.print("Stream: ");
  Serial.println(streamS);
  Serial.print("Log: ");
  Serial.println(logS);
  initSDCard();
  Wire.begin();
  compass.init();
  compass.enableDefault();
  //Wifi.mode(WIFI_STA);
  //Wifi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  //Wifi.setHostname("TremorLabs Monitor V2"); //set device name
  if (!logS) {
    Serial.println("Starting WiFi!");
    WiFi.begin(ssid, password);
    for(int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());   //Get ESP IP

    Serial.println("Starting WebServer");
    ws.onEvent(wsEvent);
    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SD, "/page.html", "text/html");
    });

    server.serveStatic("/dist/", SD, "/dist/");
    server.begin();
  } else {
    Serial.println("Logging active: no web things")
  }
  millisWS = millis(); //using millis instead of delay for more accurate timing
  millisAC = millisWS;
}


void loop() {
  currentMillis = millis(); //get elapsed millis since boot (overflow every 49 days, this should be fine)
  if ((currentMillis - millisWS >= connwait)&&(!logS)){ // no point in calling ws.cleanupClients if we don't have a websocket (server)
    ws.cleanupClients();
    //Serial.println(ESP.getFreeHeap()); //should stabilise at a value when socket connected. if this goes down eventually the weboscket will get angry and start lagging a bit
    millisWS = currentMillis;
  }
  if (currentMillis - millisAC >= freqwait){
    millisAC = currentMillis;
    if (streamS || logS) {
        compass.read();
        readings[ptr] = compass.a.x;
        readings[ptr+1] = compass.a.y;
        readings[ptr+2] = compass.a.z;
        ptr+=3;
        if (ptr>=29) {
          char outbuf[1024], *put = outbuf;
          for(int i = 0; i < sizeof readings / sizeof *readings; ++i) {
            put += snprintf(put, sizeof outbuf - (put - outbuf), "%6d ", readings[i]);
          }
          if (streamS) {
            ws.textAll(outbuf);
          }
          if (logS) {
            //log outbuf to sd card
          }
          ptr=0;
        }
        //ws.printfAll("%6d %6d %6d %6d", compass.a.x, compass.a.y, compass.a.z, currentMillis);
        //Serial.println("sent");
        //Serial.println(currentMillis);
      }
    //}
  }
}
