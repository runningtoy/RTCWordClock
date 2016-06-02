//Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELCOUNT, STRIPGPIOPIN, NEO_GRB + NEO_KHZ800);
//NeoPixelBus strip = NeoPixelBus(PIXELCOUNT, STRIPGPIOPIN, NEO_GRB + NEO_KHZ800);
//NeoPixelBus<NeoRgbwFeature, Neo800KbpsMethod> strip(PIXELCOUNT, STRIPGPIOPIN);
#ifndef LED_H
#define LED_H

#include "wordclockdef.h"

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELCOUNT, STRIPGPIOPIN, NEO_GRB + NEO_KHZ800);
//NeoPixelBus strip = NeoPixelBus(PIXELCOUNT, STRIPGPIOPIN, NEO_GRB + NEO_KHZ800);
NeoPixelBus strip = NeoPixelBus(PIXELCOUNT, STRIPGPIOPIN, NEO_GRB + NEO_KHZ800);
static RgbColor basecolor = RgbColor(128, 0, 0);

void setBASECOLOR(RgbColor color);
void setBASECOLOR(String hexstring);
RgbColor getBASECOLOR();
String getBASECOLORString();
void setStripBrightness(uint8_t Brightness);
double getEnvBrightness();
void pushLED(int pixelIDs[], int c);
void pushLED(word_info cword);
void colorWipe(uint32_t c, uint8_t wait);
void clearStrip();
void blinkStrip();
void setupSTRIP();
void tempToStrip(int temp, int menu);
void tempToStrip(int temp){tempToStrip(temp,0);}
void timeToStrip(uint8_t hours, uint8_t minutes);
void minutesToStrip(int singleMinutes);
RgbColor RandomStripColor();
void sort(int a[], int size);
static float CalcColor(float p, float q, float t);
RgbColor brightenColor(RgbColor& color, double factor);
double dblmap(double x, double in_min, double in_max, double out_min, double out_max);
RgbColor HSL2RgbColor(float h,float s,float l);
double randomDouble(double minf, double maxf);
/*
 * Array sortieren
 */
void sort(int a[], int size) {
  for (int i = 0; i < (size - 1); i++) {
    for (int o = 0; o < (size - (i + 1)); o++) {
      if (a[o] > a[o + 1]) {
        int t = a[o];
        a[o] = a[o + 1];
        a[o + 1] = t;
      }
    }
  }
}

/*
 * Config Color setzen
 */
void setBASECOLOR(RgbColor color) {
  basecolor = color;
  //VERBOSENL((String)basecolor);
//  String hexstring = "#"+getBASECOLORString();
//  config.LEDColor = strtol(  &hexstring[1], NULL, 16);
}

/*
 * Config Color lesen
 */
RgbColor getBASECOLOR() {
  return basecolor;//config.LEDColor;
}

String getBASECOLORString() {
  return String(basecolor.R, HEX) + String(basecolor.G, HEX) + String(basecolor.B, HEX);
}

void setBASECOLOR(String hexstring) { //#ffaabb
  int number = strtol( &hexstring[1], NULL, 16);
  //http://stackoverflow.com/questions/23576827/arduino-convert-a-sting-hex-ffffff-into-3-int
  // Split them up into r, g, b values
  long LED_R = number >> 16  & 0xFF;
  long LED_G = number >> 8 & 0xFF;
  long LED_B = number & 0xFF;
  basecolor = RgbColor(LED_R, LED_G, LED_B);
}

 
/*
 * LEDs setzen entsprechend der Paramter
 */
void pushLED(int pixelIDs[], int c) {
  //sort(pixelIDs, c);
  for (int i = 0; i < c; i++) {
    strip.SetPixelColor(pixelIDs[i], getBASECOLOR());
  }
}
/*
 * LEDs setzen entsprechend der Paramter
 */
void pushLED(word_info cword) {
  //delay(0);
  //sort(cword.pos, cword.len);
  for (int i = 0; i < cword.len; i++) {
    strip.SetPixelColor(cword.pos[i], getBASECOLOR());
  }
}


RgbColor RandomStripColor() {
  //randomSeed((long)(micros()*analogRead(LDRPIN)));
  //randomSeed(ESP8266_DREG(0x20E44));
  randomSeed(RANDOM_REG32);
  //return RgbColor(int(random(60,210)), int(random(60,210)), int(random(60,210)));
  return HSL2RgbColor(random(0,60)/60.0f,1,0.5);
}

// Fill the dots one after the other with a color
void colorWipe(RgbColor c, uint8_t wait) {
  for (uint16_t i = 0; i < PIXELCOUNT; i++) {
    strip.SetPixelColor(i, c);
    strip.Show();
    delay(wait);
    
  }
}

void clearStrip() {
  strip.ClearTo(RgbColor(0, 0, 0));
}


/*
 * LED Streifen blinken lassen
 */
void blinkStrip() {
  for (int i = 0; i < 3; i++) {
    clearStrip();
    strip.Show();
    delay(500);
    pushLED(_ONE);
    pushLED(_TWO);
    pushLED(_THREE);
    pushLED(_FOUR);
    strip.Show();
    delay(500);
  }
}

/*
 * Init LED Stip
 */
void setupSTRIP() {
  //boot stripe
  strip.Begin();
  strip.Show();
  //setBASECOLOR(strip.Color(config.LED_R, config.LED_G, config.LED_B));
  blinkStrip();
}

/*
 * Temperatur Anzeigen und entsprechen LED anzeigen
 */
void tempToStrip(int temp, int menu) {
  RgbColor tempcolor = getBASECOLOR();
  double linerColor=map(temp, -10, 40, 0, 100);
  //Serial.println("Value;"+(String)linerColor);
  linerColor=LIMIT(linerColor,100,0);  //Limitiere denn auf einen Wert zwischen 0 und 100
  //Serial.println("Limited;"+(String)linerColor);
  //linerColor=linerColor>100?100:linerColor;
  //linerColor=linerColor<0?0:linerColor;
  //linerColor=linerColor/100;
  setBASECOLOR((RgbColor::LinearBlend(RgbColor(0, 0, 220), RgbColor(220, 0, 0), float(linerColor/100))));
  
  int zehner = (temp / 10) % 10;
  int einer = temp  % 10;
  clearStrip();
  switch (zehner) {
    case 1: pushLED(_L1); break;
    case 2: pushLED(_L2); break;
    case 3: pushLED(_L3); break;
    case 4: pushLED(_L4); break;
    case 5: pushLED(_L5); break;
    case 6: pushLED(_L6); break;
    case 7: pushLED(_L7); break;
    case 8: pushLED(_L8); break;
    case 9: pushLED(_L9); break;
  }
  switch (einer) {
    case 0: pushLED(_R0); break;
    case 1: pushLED(_R1); break;
    case 2: pushLED(_R2); break;
    case 3: pushLED(_R3); break;
    case 4: pushLED(_R4); break;
    case 5: pushLED(_R5); break;
    case 6: pushLED(_R6); break;
    case 7: pushLED(_R7); break;
    case 8: pushLED(_R8); break;
    case 9: pushLED(_R9); break;
  }

  minutesToStrip(menu);
  
  strip.Show();
  setBASECOLOR(tempcolor);
}

void minutesToStrip(int singleMinutes) {
  //Show minutes
  switch (singleMinutes) {
    case 1:
      pushLED(_ONE);
      break;
    case 2:
      pushLED(_TWO);
      break;
    case 3:
      pushLED(_THREE);
      break;
    case 4:
      pushLED(_FOUR);
      break;
    default:
      break;
  }
}

void textMinutesToStrip(uint8_t minutes) {
  if (minutes >= 5 && minutes < 10) {
    pushLED(_FUENF_M);
    pushLED(_NACH);
  } else if (minutes >= 10 && minutes < 15) {
    pushLED(_ZEHN_M);
    pushLED(_NACH);
  } else if (minutes >= 15 && minutes < 20) {
    pushLED(_VIERTEL);
  } else if (minutes >= 20 && minutes < 25) {
    pushLED(_ZWANZIG);
    pushLED(_NACH);
  } else if (minutes >= 25 && minutes < 30) {
    pushLED(_FUENF_M);
    pushLED(_VOR);
    pushLED(_HALB);
  } else if (minutes >= 30 && minutes < 35) {
    pushLED(_HALB);
  } else if (minutes >= 35 && minutes < 40) {
    pushLED(_FUENF_M);
    pushLED(_NACH);
    pushLED(_HALB);
  } else if (minutes >= 40 && minutes < 45) {
    pushLED(_ZWANZIG);
    pushLED(_VOR);
  } else if (minutes >= 45 && minutes < 50) {
    pushLED(_DREIVIERTEL);
  } else if (minutes >= 50 && minutes < 55) {
    pushLED(_ZEHN_M);
    pushLED(_VOR);
  } else if (minutes >= 55 && minutes < 60) {
    pushLED(_FUENF_M);
    pushLED(_VOR);
  }
}

void textHoursToStrip(uint8_t hours, uint8_t minutes) {
  //Show hours
  switch (hours) {
    case 0:
      pushLED(_ZWOELF);
      break;
    case 1:
      if (minutes > 4) {
        pushLED(_EINS);
      } else {
        pushLED(_EIN);
      }
      break;
    case 2:
      pushLED(_ZWEI);
      break;
    case 3:
      pushLED(_DREI);
      break;
    case 4:
      pushLED(_VIER);
      break;
    case 5:
      pushLED(_FUENF_H);
      break;
    case 6:
      pushLED(_SECHS);
      break;
    case 7:
      pushLED(_SIEBEN);
      break;
    case 8:
      pushLED(_ACHT);
      break;
    case 9:
      pushLED(_NEUN);
      break;
    case 10:
      pushLED(_ZEHN_H);
      break;
    case 11:
      pushLED(_ELF);
      break;
  }
}

/*
 * Zeit Schreibefunktion
 *
 * setzen der einzelnen LEDS, entsprechend der Zeit
 */
void timeToStrip(uint8_t hours, uint8_t minutes)
{
  RgbColor tempcolor = getBASECOLOR();
  setBASECOLOR(getBASECOLOR());
  pushLED(_ES);
  pushLED(_IST);
  //Log.info("fct:timeToStrip %d:%d",hours,minutes);

  minutesToStrip(minutes % 5);
  textMinutesToStrip(minutes);
  

  //calc hours
  if (hours >= 12) {
    hours -= 12;
  }

  if ((minutes >= 15 && minutes < 20) || minutes >= 25)  {
    hours++;
  }

  if (hours == 12) {
    hours = 0;
  }

  textHoursToStrip(hours, minutes);
  
  //Show uhr
  if (minutes < 5) {
    pushLED(_UHR);
  }

  setBASECOLOR(tempcolor);
}

static float CalcColor(float p, float q, float t)
{
    if (t < 0.0f)
        t += 1.0f;
    if (t > 1.0f)
        t -= 1.0f;

    if (t < 1.0f / 6.0f)
        return p + (q - p) * 6.0f * t;

    if (t < 0.5f)
        return q;

    if (t < 2.0f / 3.0f)
        return p + ((q - p) * (2.0f / 3.0f - t) * 6.0f);

    return p;
}

double dblmap(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double randomDouble(double minf, double maxf) //http://forum.arduino.cc/index.php?topic=371564.0
{
  return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31);  // use 1ULL<<63 for max double values)
}

RgbColor HSL2RgbColor(float h,float s,float l)
{
    float r;
    float g;
    float b;

    if (s == 0.0f || l == 0.0f)
    {
        r = g = b = l; // achromatic or black
    }
    else 
    {
        float q = l < 0.5f ? l * (1.0f + s) : l + s - (l * s);
        float p = 2.0f * l - q;
        r = CalcColor(p, q, h + 1.0f / 3.0f);
        g = CalcColor(p, q, h);
        b = CalcColor(p, q, h - 1.0f / 3.0f);
    }
    
    //VERBOSELN(String(__PRETTY_FUNCTION__) + ":: R:"+ (String)(uint8_t)(r * 255.0f) + " G:" + (String)(uint8_t)(g * 255.0f) + " B:" + (String)(uint8_t)(b * 255.0f));
    return RgbColor((uint8_t)(r * 255.0f),(uint8_t)(g * 255.0f),(uint8_t)(b * 255.0f));
}

#endif




