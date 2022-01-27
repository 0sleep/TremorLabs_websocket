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


# stuff for esp32
https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver/ <- manage wifi with creds stored in eeprom
https://www.mischianti.org/2020/06/04/esp32-integrated-spiffs-filesystem-part-2/ <- you have 4 mb
https://randomnerdtutorials.com/esp32-web-server-microsd-card/ <- but you can serve files off sd card
https://github.com/me-no-dev/ESPAsyncWebServer/issues/1085 <- manual fix for ESPAsyncWebServer (arduino update broke it :/)
https://makeradvisor.com/tools/sd-card-module/ <- cheap af reader
https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/ <- interrups might be useful
https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573 <- use this instead of delay! [done]
