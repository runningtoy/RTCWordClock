#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <FS.h>
#include <DNSServer.h>
ESP8266WebServer server(80);
#include "OTA.h"

/* Set these to your desired credentials. */
const String ssid = "WordClock-" + String(ESP.getChipId(),HEX);
const String password = (String)ESP.getChipId();

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;

void handleRoot();
void serverSetup();
void serverStop();
String fileDir();
void  handleDownload();
bool handleFileRead(String path);
String getContentType(String filename);
String formatBytes(size_t bytes);
String urldecode(String input);
unsigned char h2int(char c);
void checkArguments();
String getValue(String data, char separator, int index);
String currentTimeHTML();  


void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}


void serverSetup() {
  SPIFFS.begin(); 
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  //WiFi.softAP(ssid.c_str(), password.c_str());
  WiFi.softAP(ssid.c_str());

  dnsServer.start(DNS_PORT, "*", apIP);
  
  IPAddress myIP = WiFi.softAPIP();
  String str = "AP IP address: " + myIP.toString();
  int str_len = str.length() + 1; 
  char myIP_array[str_len];
  str.toCharArray(myIP_array, str_len);
  Log.info(myIP_array);


  //OTA
  server.onFileUpload(handleFileUpload);
  server.on("/update", handleUpdate);
  server.on("/upload", handleUpload);
  //use it to load content from SPIFFS
  
  server.onNotFound([]() {
    String filepath=server.uri();
    if (server.args() > 0 ){
      checkArguments();
      filepath="/index.html";
    };  
    if (!handleFileRead(filepath))
        server.send(404, "text/plain", "FileNotFound");
  });
  server.on ( "/debug/ls", []() {
    server.send ( 200, "text/plain", fileDir());
  } );
  server.on ( "/debug/time", []() {
    server.send ( 200, "text/plain", currentTimeHTML());
  } );
  server.on ( "/debug", []() {
    String s="";
    s+="<a href=http://" +  apIP.toString() +"/debug/ls>ls</a><br>";
    s+="<a href=http://" +  apIP.toString() +"/debug/format>format</a><br>";
    s+="<a href=http://" +  apIP.toString() +"/debug/time>time</a><br>";
    s+="<a href=http://" +  apIP.toString() +">index</a><br>";
    s+="update:: curl -F \"image=@firmware.bin\" "+apIP.toString()+"/update<br>";
    server.send ( 200, "text/html", s);
  } );
  //server.on("/download", handleDownload); 
  
  server.on ( "/debug/format", []() {
    SPIFFS.format();
    Log.info("FileSystem formated");
  } );
  
  
  server.begin();
  Log.info("HTTP server started");
}

void serverStop(){
  server.stop();
  WiFi.softAPdisconnect(true);
  Log.info("Server Stopped and closed");
}



void serverLoop(){
  server.handleClient();
  dnsServer.processNextRequest();
}

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  //if (path.endsWith("/")) path += "index.html";
  //if (path.endsWith("hotspot-detect.html")) path = "/index.html";
  path = "/index.html";
  
  char charBuf[50];
  path.toCharArray(charBuf, 50); 
  Log.info(charBuf);
  
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

////http://www.esp8266.com/viewtopic.php?f=32&t=4570
////http://<esp_ipaddr>/download?file=/Config.txt
void  handleDownload(){
  if (!SPIFFS.begin()) {
    Serial.println((String)"SPIFFS failed to mount !\r\n");                   
  }
  else {
    String str = "";
    File f = SPIFFS.open(server.arg(0), "r");
    //Log.verbose(((String)server.arg(0)).c_str());
    if (!f) {
       Log.error("Can't open SPIFFS file !");         
    }
    else {
      char buf[1024];
      int siz = f.size();
      while(siz > 0) {
        size_t len = std::min((int)(sizeof(buf) - 1), siz);
        f.read((uint8_t *)buf, len);
        buf[len] = 0;
        str += buf;
        siz -= sizeof(buf) - 1;
      }
      f.close();
      server.send(200, "text/plain", str);
    }
  }
} 

String fileDir() {
  Dir dir = SPIFFS.openDir("/");
  String ret = "";
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    ret = ret + "FS File: " + fileName + ", size: " + formatBytes(fileSize) + "\n";
  }
  return ret;
} 

String currentTimeHTML() {
  String ret = "Time: ";
  RtcDateTime dt=RTCnow();
  ret=ret+(String)dt.Day()+"."+(String)dt.Month()+"."+(String)dt.Year()+" "+(String)dt.Hour()+":"+(String)dt.Minute()+":"+(String)dt.Second()+"\n";
  return ret;
} 


void checkArguments()
{
  //file:///C:/Users/f40759/Downloads/simple-flat-contact-form/index.html?time=18%3A12%3A18&data=12.12.2015&daylight=on
  if (server.args() > 0 )  // Save Settings
  {
    String temp = "";
    config.daylight=false;
    String time="",date="";
    bool _t=false,_d=false;
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "time"){time=urldecode(server.arg(i)); _t=true;}
      if (server.argName(i) == "date"){date=urldecode(server.arg(i)); _d=true;}
      if (server.argName(i) == "daylight"){config.daylight=true;}
      if (server.argName(i) == "color") {
        setBASECOLOR(urldecode(server.arg(i)));  
        config.randomcolor=false;
        Log.info("Change Color");
      }
      delay(1);
    }

    if(_t && _d)
    {
      RtcDateTime dt=RtcDateTime((uint16_t)atoi(getValue(date,'.',2).c_str()),(uint8_t)atoi(getValue(date,'.',1).c_str()),(uint8_t)atoi(getValue(date,'.',0).c_str()),(uint8_t)atoi(getValue(time,':',0).c_str()),(uint8_t)atoi(getValue(time,':',1).c_str()),(uint8_t)atoi(getValue(time,':',2).c_str()));
      config.inittime=dt;
      setRTCTime(dt);
    }
    
  }
  Log.info("checkArguments"); 
} 
 
// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

String urldecode(String input) // (based on https://code.google.com/p/avr-netino/)
{
   char c;
   String ret = "";
   
   for(byte t=0;t<input.length();t++)
   {
     c = input[t];
     if (c == '+') c = ' ';
         if (c == '%') {


         t++;
         c = input[t];
         t++;
         c = (h2int(c) << 4) | h2int(input[t]);
     }
    
     ret.concat(c);
   }
   return ret;
  
}


String getValue(String data, char separator, int index)
{
 int found = 0;
  int strIndex[] = {
0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

