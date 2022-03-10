# ffs.ino
The following program will take the accelerometer data and log it to the serial port
```cpp
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
```
## Explanation
```cpp
#include "Wire.h"
#include "LSM303.h"
```
These lines add the required libraries, the important one being LSM303 to read data from the accelerometer

```cpp
LSM303 sensor;
```
We create a sensor object, which lets us interface with the accelerometer (connected via i2c)

```cpp
char report[80];
```
Report is the string that will get sent to the serial port
```cpp
void setup() {
  ...
}
```
The setup function will run once, when the board boots up
```cpp
Serial.begin(115200);
```
This will open the serial connection to the computer at a baud rate (=speed) of 115200
```cpp
Wire.begin();
```
This tells the Wire library to start (Wire.h handles i2c connections)
```cpp
sensor.init()
sensor.enableDefault()
```
Here we tell the sensor to start
```cpp
void loop() {
  ...
}
```
The loop function gets called repeatedly, once the setup function has finished
```cpp
sensor.read();
```
This tells the accelerometer to read the current acceleration. The readings are stored in the field of the sensor object and can be accessed with `sensor.a.x` (for example)
```cpp
snprinf(report, sizeof(report), "%6d,%6d,%6d", sensor.a.x, sensor.a.y, sensor.a.z)
```
The snprintf function is used for string formatting. The first two arguments are the variable it should write to and the length of the variable. The next argument is a template string into which snprintf should place the values `sensor.a.x, sensor.a.y, sensor.a.z`. %6d is a placeholder for an integer
```cpp
Serial.println(report);
```
Prints the data (which will look like `-15644,761,-5964`) to the serial port
```cpp
delay(10);
```
Waits for 10 ms
# sd_logging.ino

This program will take the data and put it on the SD card
```cpp
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <LSM303.h>

LSM303 sensor;
char* outS = new char[300];

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
                delay(20);
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
```
## Explanation
```cpp
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Wire.h"
#include <LSM303.h>
```
Here we include the Wire and LSM303 libraries, just like in the previous program, but also the FS, SD and SPI libraries, as these handle the SD card communication and filesystem

```cpp
LSM303 sensor;
```
Create an LSM303 object named sensor to interface with the LSM303D accelerometer
```cpp
char* outS = new char[300];
```
Create a string which will contain a buffer of measurements to be written to the SD card (note: this is already a string and not an array of integers)
```cpp
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
```
Set up the SD card and print any problems to the serial port
```cpp
Wire.begin();
sensor.init();
sensor.enableDefault();
```
Initialise the accelerometer
```cpp
int len = 0;
```
Set the "pointer" to 0 (this variable keeps track of how far we have progressed through our buffer)
```cpp
for (int i=0; i<100; i++) {
  ...
}
```
Take 100 readings, at 50 Hz sample rate this would be 2 seconds of data
```cpp
sensor.read();
```
Read the current acceleration
```cpp
len += sprintf(outS + len, "%6d, %6d, %6d\n", sensor.a.x, sensor.a.y, sensor.a.z);
```
write our x y and z acceleration (seperated by ",") into our buffer at position len and increment len to reflect new position
```cpp
delay(20);
```
Pause for 20 ms. Ideally this would give us a sample rate of 50 Hz, however the previous lines of code will take a while to run, so it might not be entirely accurate
```cpp
appendFile(SD, "/data.csv", outS);
```
This writes our data (outS) to data.csv on the SD card
```cpp
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
```
This function takes our SD object, a path to a file and a string (char*) and writes to the card. This function was written by Rui Santos (https://randomnerdtutorials.com/esp32-microsd-card-arduino/)
