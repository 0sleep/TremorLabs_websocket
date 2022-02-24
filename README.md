# Websockets!
## connection
(normal)

esp32 <--- wifi ---> Wifi AP <--- network ---> computer
C <--- data transfer via websocket protocol ---> webpage
websocket server                                client
client              wifi server                 client


(setup)
esp32 <--- wifi ---> computer
webserver            client

## features ideally
* real time data to webpage via websockets (stream or packet?)
* fast fourier transforms to get frequency
* wifi setup and settings storage in eeprom
* multiple clients?
* data storage / file save?

## what works currently
* esp connects to wifi AP
* esp can be websocket server
* esp sends accelerometer data every second to newest client
asas

# stuff for esp32
https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver/ <- manage wifi with creds stored in eeprom
https://www.mischianti.org/2020/06/04/esp32-integrated-spiffs-filesystem-part-2/ <- you have 4 mb
https://randomnerdtutorials.com/esp32-web-server-microsd-card/ <- but you can serve files off sd card [done]
https://github.com/me-no-dev/ESPAsyncWebServer/issues/1085 <- manual fix for ESPAsyncWebServer (arduino update broke it :/) [done]
https://makeradvisor.com/tools/sd-card-module/ <- cheap af reader
https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/ <- interrupts might be useful
https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573 <- use this instead of delay! [done]
https://github.com/me-no-dev/ESPAsyncWebServer#async-websocket-event <- use integrated websocket event from ESPAsyncWebServer! [done]
https://github.com/me-no-dev/ESPAsyncWebServer/issues/504 <- oh no :( can only send 15 readings over websocket per second...

# TODO
- use integrated websocket thing in ESPAsyncWebServer to hopefully speed things up. currently rather slow [done]
  - still slow, max messages per sec is about 15 for esp32, fix:
    - batch messages together (may fill heap fast)
    - events? (not bi-directional...)
- read wifi connection data from sd card
- rgb status led for charge, wifi conn, ...
