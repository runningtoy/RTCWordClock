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
[Logging](https://github.com/mrRobot62/Arduino-logging-library), [NeoPixelBus](https://github.com/Makuna/NeoPixelBus), [RTC](https://github.com/Makuna/Rtc)
