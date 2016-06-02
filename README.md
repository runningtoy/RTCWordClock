#RTC WordClock
A german esp8266 WordClock using an RTC Chip and WS2812b LEDs

## Using
On Powerup an Accesspoint is setup. If connected http://192.168.1.1 lead to the settings Page, http://192.168.1.1/debug leads to a information site. After 10minutes the AP is disabled.  
On the Settings page you can set the date and time and if you want a static color you can choose it here. (If not selected, the color changes each hour)


## OTA is available, use cUrl
curl -F "image=@firmware.bin" http://192.168.1.1/update

## wordclockdef.h
Is the definition File for the LEDs, change here for your LEDs

## Needed Libraries

- [Logging](https://github.com/mrRobot62/Arduino-logging-library)
- [NeoPixelBus](https://github.com/Makuna/NeoPixelBus)
- [RTC](https://github.com/Makuna/Rtc)

tested on Arduino 1.6.8 and [Arduino core for ESP8266 WiFi chip (Release 2.2.0)](https://github.com/esp8266/Arduino/releases/tag/2.2.0)


#Hardware

- [Wemos D1 mini](http://www.wemos.cc)
- WS2812b LEDs
- DS3231 breakout board
![RTC_WordClock_Steckplatine.png](https://github.com/runningtoy/RTCWordClock/blob/master/RTC_WordClock_Steckplatine.png)
