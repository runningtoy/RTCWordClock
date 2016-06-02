#include <Logging.h>
#include <NeoPixelBus.h>
#include <RtcDS3231.h>
#include "config.h"
#include "clock.h"
#include <Ticker.h>
#include "LED.h"
#include "webserver.h"


boolean webserveractiv=true;
boolean _randomColorH=false;

Ticker clockTicker;

void setup() {
  Log.init(LOGLEVEL, 115200); 
  setupClock();
  printDateTime(RTCnow());
  setupSTRIP();
  blinkStrip();

  //webserver aktiviren
  if(webserveractiv)
  {
    serverSetup();
  }

  // put your setup code here, to run once:
  clockTicker.attach(15, fct_ClockTicker);
}

void loop() {
  if(millis()>m_minute*SERVERUPTIME && webserveractiv)
  {
    webserveractiv=false; 
    serverStop();
    blinkStrip();
    clearStrip();
    timeToStrip((uint8_t)RTCnow().Hour(), (uint8_t)RTCnow().Minute());
    strip.Show();
    //AP schließen, server beenden aufräumen,... 
  }
  
  if(webserveractiv)
  {
      serverLoop();
  }
}


void fct_ClockTicker(){
  clearStrip();
  timeToStrip((uint8_t)RTCnow().Hour(), (uint8_t)RTCnow().Minute());
  strip.Show();
  if(RTCnow().Minute()%59==0 && _randomColorH && config.randomcolor)
  {
    _randomColorH=false; 
    Log.info("Change Color");
    setBASECOLOR(RandomStripColor());
  }
  if(RTCnow().Minute()%59!=0)
  {
    _randomColorH=true;
  }
  printDateTime(RTCnow());
}

