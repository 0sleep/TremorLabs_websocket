#include <Wire.h>
#include <LSM303.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>

const char* ssid = "WR-32-srv"; //Enter SSID
const char* password = "50N6-99s"; //Enter Password
//String devicename = "TremorLabs Monitor V2"; //what we want the device to "look" like

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long connwait = 10;
const unsigned long freqwait = 100;

using namespace websockets;

WebsocketsServer server;
LSM303 compass;
char report[80];
WebsocketsClient client;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  Serial.println("Starting WiFi");
  Wifi.mode(WIFI_STA);
  Wifi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  Wifi.setHostname("TremorLabs Monitor V2"); //set device name
  WiFi.begin(ssid, password);
  // Wait some time to connect to wifi
  for(int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
      Serial.print(".");
      delay(1000);
  }
  startMillis = millis(); //using millis instead of delay for more accurate timing
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //Get ESP IP

  server.listen(80);
  Serial.print("Server live: ");
  Serial.println(server.available());
}


void loop()
{
  currentMillis = millis(); //get elapsed millis since boot (overflow every 49 days, this should be fine)
  if (currentMillis - startMillis >= connwait){
    if (server.poll()) {
      client = server.accept();
      Serial.println("client accepted");
    }
  }
  if (currentMillis - startMillis >= freqwait){
    if(client.available()) {
      Serial.println("client available");
      compass.read();
      snprintf(report, sizeof(report), "%6d %6d %6d",
          compass.a.x, compass.a.y, compass.a.z);
      Serial.println(report);
      client.send(report);
    }
  }
}
