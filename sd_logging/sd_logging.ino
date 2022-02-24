#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Wire.h"
#include <LSM303.h>

LSM303 sensor;
char* outS = new char[300]; //string to hold our data. one could have it as an array and then convert to string, but this is probably more efficient

void setup() {
        Serial.begin(115200);
        if(!SD.begin(5)) {
                Serial.println("Card Mount Failed");
                return;
        }
        uint8_t cardType = SD.cardType();
        if(cardType == CARD_NONE) {
                Serial.println("No SD card attached");
                return;
        }
        Wire.begin();
        sensor.init();
        sensor.enableDefault();
}
void loop() {
        int len = 0;
        for (int i=0; i<100; i++) {
                sensor.read();
                len += sprintf(outS + len, "%6d, %6d, %6d\n", sensor.a.x, sensor.a.y, sensor.a.z);
                delay(20); //read approx every 20 ms, as accelerometer's max frequency is 50 hz (until i mess with the control registers)
        }
        appendFile(SD, "/data.csv", outS);
}
void appendFile(fs::FS &fs, const char * path, const char * message){
        Serial.printf("Appending to file: %s\n", path);

        File file = fs.open(path, FILE_APPEND);
        if(!file) {
                Serial.println("Failed to open file for appending");
                return;
        }
        if(file.print(message)) {
                Serial.println("Message appended");
        } else {
                Serial.println("Append failed");
        }
        file.close();
}
