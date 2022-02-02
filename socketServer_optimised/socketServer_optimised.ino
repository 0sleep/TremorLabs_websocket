#include <Arduino.h>
#include <Wire.h>
#include <LSM303.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

const char* ssid = "WR-32-srv"; //Enter SSID
const char* password = "50N6-99s"; //Enter Password
//String devicename = "TremorLabs Monitor V2"; //what we want the device to "look" like
long readings[30]; //sensor readings batch, to try and limit the messages per second. might not actually help...
int ptr = 0; //points to current position in readings
int mode = 0; // 0: idle, 1: stream to page, 2: stream to sd

unsigned long millisWS;
unsigned long millisAC;
unsigned long currentMillis;
const unsigned long connwait = 200;
const unsigned long freqwait = 20;

LSM303 compass;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void initSDCard(){
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

void wsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type==WS_EVT_CONNECT) {
    Serial.println("Connection received");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Connection terminated");
  }
}

void setup() {
  Serial.begin(115200);
  initSDCard();
  Wire.begin();
  compass.init();
  compass.enableDefault();
  Serial.println("Starting WiFi");
  //Wifi.mode(WIFI_STA);
  //Wifi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  //Wifi.setHostname("TremorLabs Monitor V2"); //set device name
  WiFi.begin(ssid, password);
  for(int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
      Serial.print(".");
      delay(1000);
  }
  millisWS = millis(); //using millis instead of delay for more accurate timing
  millisAC = millis();
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
}


void loop()
{
  currentMillis = millis(); //get elapsed millis since boot (overflow every 49 days, this should be fine)
  if (currentMillis - millisWS >= connwait){
    ws.cleanupClients();
    char outbuf[1024], *put = outbuf;
    for(int i = 0; i < sizeof readings / sizeof *readings; ++i) {
      put += snprintf(put, sizeof outbuf - (put - outbuf), "%6d ", readings[i]);
    }
    ws.textAll(outbuf);
    ptr=0;
    Serial.println(ESP.getFreeHeap()); //should stabilise at a value when socket connected. if this goes down eventually the weboscket will get angry and start lagging a bit
    millisWS = currentMillis;
  }
  if (currentMillis - millisAC >= freqwait){
      compass.read();
      readings[ptr] = compass.a.x;
      readings[ptr+1] = compass.a.y;
      readings[ptr+2] = compass.a.z;
      ptr+=3;
      //ws.printfAll("%6d %6d %6d", compass.a.x, compass.a.y, compass.a.z);
      //Serial.println("sent");
      //Serial.println(currentMillis);
      millisAC = currentMillis;
    //}
  }}
