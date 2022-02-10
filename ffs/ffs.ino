#include "Arduino.h"
#include "Wire.h"
#include "LSM303.h"

LSM303 sensor;
char report[80];

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sensor.init();
  sensor.enableDefault();
}

void loop() {
  sensor.read();
  snprintf(report, sizeof(report), "%6d,%6d,%6d",
      sensor.a.x, sensor.a.y, sensor.a.z);
  Serial.println(report);
  delay(10);
}
