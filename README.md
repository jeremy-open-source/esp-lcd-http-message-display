# ESP LCD HTTP Message Display

This is an Arduino IDE Sketch I created for displaying message on my boards below (also see references below) 

## Notes:
* As indicated in the Robot Zero article (1) (see Links/References), you need to:
"Edit `libraries/U8g2_Arduino/src/clib/u8g2.h` and uncomment the line:`// #define U8G2_16BIT`" 
* As indicated in the top of the Sketch, I have used some additional libraries (also see link 1)
* I have created a simple test script `bin/test/send-message.sh`


## Boards tested
* HTIT-W8266 (Heltec Internet of things WiFi Kit 8)
* DK-W8266 (Deek-Robot)

## Links / References
* (1) https://robotzero.one/heltec-wifi-kit-8/
