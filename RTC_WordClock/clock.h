

RtcDateTime RTCnow();
void setupClock();
char* printDateTime(const RtcDateTime& dt);
void setRTCTime(uint32_t secondsFrom2000);
void setRTCTime(RtcDateTime dt);
void setRTCTime(uint16_t year,uint8_t month,uint8_t dayOfMonth,uint8_t hour, uint8_t minute, uint8_t second);
boolean summertime(int year, byte month, byte day, byte hour, byte tzHours);
boolean summertimeNow();

RtcDS3231 Rtc;


void setupClock(){
   Rtc.Begin(PINSDA,PINSCL);
   RtcDateTime compiled = config.inittime;//RtcDateTime(__DATE__, __TIME__);
   //setRTCTime(compiled);
    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Log.info("RTC lost confidence in the DateTime!");

        // following line sets the RTC to the date & time this sketch was compiled
        // it will also reset the valid flag internally unless the Rtc device is
        // having an issue

        setRTCTime(compiled);
    }

    if (!Rtc.GetIsRunning())
    {
        Log.info("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Log.info("RTC is older than compile time!  (Updating DateTime)");
        setRTCTime(compiled);
    }
    else if (now > compiled) 
    {
        Log.info("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Log.info("RTC is the same as compile time! (not expected but all is fine)");
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
}

RtcDateTime RTCnow(){
  RtcDateTime dt=Rtc.GetDateTime();
  if(summertimeNow())
    dt=dt+60*60;
  return dt;
}

char* printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            sizeof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Log.info(datestring);
    return datestring;
}



void setRTCTime(uint32_t secondsFrom2000){
  setRTCTime(RtcDateTime(secondsFrom2000));
}

void setRTCTime(RtcDateTime dt){
  if(summertime(dt.Year(), dt.Month(), dt.Day(), dt.Hour(),0)){
    dt=dt-60*60; 
  }
  Log.info("setRTCTime");
  printDateTime(dt);  
  Rtc.SetDateTime(dt);
}

void setRTCTime(uint16_t year,uint8_t month,uint8_t dayOfMonth,uint8_t hour, uint8_t minute, uint8_t second){
  setRTCTime(RtcDateTime(year,month,dayOfMonth,hour, minute, second));
}


//
// Summertime calculates the daylight saving for a given date.
//
boolean summertime(int year, byte month, byte day, byte hour, byte tzHours)
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)
{
 if (month<3 || month>10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
 if (month>3 && month<10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
 if (month==3 && (hour + 24 * day)>=(1 + tzHours + 24*(31 - (5 * year /4 + 4) % 7)) || month==10 && (hour + 24 * day)<(1 + tzHours + 24*(31 - (5 * year /4 + 1) % 7)))
   return true;
 else
   return false;
} 

boolean summertimeNow(){
  RtcDateTime dt=Rtc.GetDateTime();
  return summertime(dt.Year(), dt.Month(), dt.Day(), dt.Hour(),0);
}

