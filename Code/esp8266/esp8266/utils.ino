String getUniqueID(){
    String idTmp = String(ESP.getChipId(), HEX);
  for(int i=0; i<(8-idTmp.length()); i++) {
    uniqueID += '0';
  }
  uniqueID += idTmp;
  return uniqueID;
}

void systemInformation(){
  // ToDo: Find a better way to also send it over telnet
  printDebug("IP address: " + String(WiFi.localIP()));
  printDebug("Host: " + String(host) + ".local");
  printDebug("esp-chip-id: " + uniqueID);
  printDebug("MAC: " + String(WiFi.macAddress()));
  printDebug("AP-MAC: " + String(WiFi.softAPmacAddress()));
  printDebug("free-space: " + String(ESP.getFreeSketchSpace()));
  printDebug("sketch-size: " + String(ESP.getSketchSize()));
  printDebug("flash-chip-id: " + String(ESP.getFlashChipId()));
  printDebug("flash-chip-size: " + String(ESP.getFlashChipRealSize()));
  printDebug("sdk-version: " + String(ESP.getSdkVersion()));
  printDebug("build-date-time: " + String(__DATE__) + " " + String(__TIME__));
}

void checkOTA() {
  //OTA Sketch
  if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size   = OTA.parseInt();

    Serial.print("Update Start: ip:");
    Serial.print(remote);
    Serial.printf(", port:%d, size:%d\n", port, size);
    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(size)){
      Serial.println("Update Begin Error");
      return;
    }

    WiFiClient client;
    if (client.connect(remote, port)) {

      uint32_t written;
      while(!Update.isFinished()){
        written = Update.write(client);
        if(written > 0) client.print(written, DEC);
      }
      Serial.setDebugOutput(false);

      if(Update.end()){
        client.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(client);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }
  }
  
  //IDE Monitor (connected to Serial)
  if (TelnetServer.hasClient()){
    if (!Telnet || !Telnet.connected()){
      if(Telnet){ 
        Telnet.stop();
      }
      Telnet = TelnetServer.available();
      // Created Telnet connection
      systemInformation(); // Continous connections with IDE open?!?      
    } else {
      WiFiClient toKill = TelnetServer.available();
      toKill.stop();
    }
  }
  
  if (Telnet && Telnet.connected() && Telnet.available()){
    while(Telnet.available()){
      Serial.write(Telnet.read());
    }
  }
}

void printDebug(const char* c){
  Serial.println(c);
  
  if (Telnet && Telnet.connected()){
    Telnet.println(c);
  }
}

void printDebug(String &t){
  printDebug(t.c_str());
}

