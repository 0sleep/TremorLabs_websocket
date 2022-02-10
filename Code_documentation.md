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
