#define LOGLEVEL LOG_LEVEL_DEBUG

#define SERVERUPTIME 10

#define STRIPGPIOPIN D1
#define PIXELCOUNT 128

#define PINSDA D4
#define PINSCL D3
// DS1307 SDA --> SDA
// DS1307 SCL --> SCL
// DS1307 VCC --> 5v
// DS1307 GND --> GND


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define LIMIT(V, U, L) MAX(MIN(V, U), L)   //Limt V to U..uper Value und L.. 

#define fToInt(a) (int)(a+.5)
#define millsToseconds(m)  (m/1000)%60
#define millsTominutes(m)  ((m-((m/1000)%60))/1000)/60
#define fToInt(a) (int)(a+.5)

#define m_day 86400000 // 86400000 milliseconds in a day
#define m_hour 3600000 // 3600000 milliseconds in an hour
#define m_minute 60000 // 60000 milliseconds in a minute
#define m_second 1000 // 1000 milliseconds in a second 


struct strConfig {
  boolean daylight=true;
  boolean randomcolor=true;
  //RtcDateTime inittime=RtcDateTime(__DATE__, __TIME__)+10;
  RtcDateTime inittime=RtcDateTime(2016,5,30,17,33,0)+10;
 }  config;
