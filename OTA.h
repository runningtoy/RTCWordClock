void  handleUpdate(){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError())?"UPDATED FAILED":"Update Sucessful");
      ESP.restart();
}

void  handleUpload() {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Upload");
}

void handleFileUpload() {
  String sLog="";
  char charBuf[500];
  
  if (server.uri() == "/upload") {
    static File * fsUploadFile;
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
      if (fsUploadFile) {
        delete fsUploadFile;
        fsUploadFile = nullptr;
      };
      fsUploadFile = new File;
      String filename = upload.filename;
      filename.trim();
      if (!filename.startsWith("/")) filename = "/" + filename;
      // ESPMan_Debug("handleFileUpload Name: "); ESPMan_Debugln(filename);
      sLog="Upload Name: " + filename;
      sLog.toCharArray(charBuf, 500); 
      Log.info(charBuf);
      
      if (SPIFFS.exists(filename)) SPIFFS.remove(filename);

      *fsUploadFile = SPIFFS.open(filename, "w+");

      filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (*fsUploadFile) {
        fsUploadFile->write(upload.buf, upload.currentSize);
        Log.info(".");
      };
    } else if (upload.status == UPLOAD_FILE_END) {
      String filename = String(fsUploadFile->name());
      fsUploadFile->close();
      if (fsUploadFile) {
        delete fsUploadFile;
        fsUploadFile = nullptr;
        sLog=("\nDone Size: " + (String)upload.totalSize);
        sLog.toCharArray(charBuf, 500); 
        Log.info(charBuf);
      } else {
         Log.error("Error");
      };
    } else if (upload.status == UPLOAD_FILE_ABORTED) {
      Log.info("\nAborted");
      String filename = String(fsUploadFile->name());
      fsUploadFile->close();
      if (fsUploadFile) {
        delete fsUploadFile;
        fsUploadFile = nullptr;
      }
      if (SPIFFS.exists(filename)) {
        SPIFFS.remove(filename);
      }
    }
  }
  if (server.uri() == "/update") {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      //Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      sLog=("Update: " + upload.filename);
      sLog.toCharArray(charBuf, 500); 
      Log.info(charBuf);
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
      //pushUpdate('s');
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Serial.print(".");
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
      //pushUpdate('p');
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        sLog=("\nUpdate Success: " + String(upload.totalSize, DEC) + "\nRebooting...\n");
        sLog.toCharArray(charBuf, 500); 
        Log.info(charBuf);
        //pushUpdate('d');
      } else {
        Update.printError(Serial);
        //pushUpdate('f');
      }
      Serial.setDebugOutput(false);
    }
    yield();
  }
  return;
}
 

