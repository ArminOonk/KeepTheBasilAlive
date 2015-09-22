#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include "private.h"
////////////////////////////////////////
// Chirp capacitance < 600 is dry <500 is almost dead
//
//
//
//
//
////////////////////////////////////////
// OTA, mdns
String myHostname = "";
String uniqueID;// HEX string 
const uint16_t aport = 8266;

WiFiServer TelnetServer(aport);
WiFiClient Telnet;
WiFiUDP OTA;

// Temperature
OneWire  ds(5);  // on pin D4 LoLin board 
bool ds18b20Found = false;

// #define THINGSPEAK_KEY ABCDEFGH
char host[] = "api.thingspeak.com";
String GET = "/update?api_key=" + String(THINGSPEAK_KEY) + "&field1=";
const int updateTimeout = 15*60*1000; // Thingspeak update rate
const int rebootTimeout = 1000*60*60*6;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  // Setting up OTA
  uniqueID = getUniqueID();
  myHostname = "chirp-"+uniqueID;
 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(10);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  systemInformation();

  // Start OTA
  MDNS.begin(myHostname.c_str());
  MDNS.addService("arduino", "tcp", aport);
  OTA.begin(aport);
  TelnetServer.begin();
  TelnetServer.setNoDelay(true);
    
  initTemperature();

  ESP.wdtEnable(0);
  Wire.begin(2, 14);
  resetChirp(); //reset
}

int value = 0;
long nextTimeReport = 0;
long lastTimeReport = 0;
void SendThingspeak(float temperature, float capacitance, float chirpTemp, float light){
  printDebug("connecting to " + String(host));
    
  WiFiClient client;
  const int httpPort = 80;
  if (client.connect(host, httpPort)) 
  {
    String url = GET + String(temperature);
    url += "&field2=" + String(0.001*millis());
    url += "&field3=" + String(capacitance);
    url += "&field4=" + String(chirpTemp);
    url += "&field5=" + String(light);
    
    printDebug("Requesting URL: " + String(url));
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
    delay(10);
    // Read all the lines of the reply from server and print them to Serial
    int lineNr = 0;
    while(client.available()){
      String line = client.readStringUntil('\r');
      line.trim();
      
      if(line.startsWith("Status: ")) {
        String statusCode = line.substring(8,12);
        if(statusCode.toInt() == 200) {
          printDebug("Data received");
          lastTimeReport = millis();
        }
      }
        
      Serial.println("#" + String(lineNr) + ": " + line);
      lineNr++;
      yield();
    }
      
      Serial.println();
      Serial.println("closing connection");

      nextTimeReport = millis() + updateTimeout;
    
  } else {
    printDebug("connection failed");
  }
}

int lastLoopTime = 0;
const int loopTimeout = 1000;
float temperature;
unsigned int chirpCapacitance;
unsigned int chirpTemperature;
unsigned int chirpLight;
      
void loop() {
  if(millis() - lastLoopTime > loopTimeout) {
    lastLoopTime = millis();
    
    if(ds18b20Found) {
      readAllChirp(temperature, chirpCapacitance, chirpTemperature, chirpLight);

      String report = "Chirp> Capacitance: " + String(chirpCapacitance) + " Temperature: " + String(chirpTemperature) + " Light: " + String(chirpLight);
      printDebug(report);
      
      if(millis() > nextTimeReport)
      {
        if(WiFi.status() != WL_CONNECTED) {
          Serial.println("Wifi not connected -> Restarting");
          ESP.restart();
        }
        
        SendThingspeak(temperature, chirpCapacitance, chirpTemperature, chirpLight);
      }
      printDebug(String(value) + ": Temperature = " + String(temperature));
    
    } else {
      printDebug("Count: " + String(value));
    }
    value++;
  
    if(millis() > (lastTimeReport + 2*updateTimeout)) {
      Serial.println("No server response -> Restarting");
      ESP.restart();
    }
  }
  
  checkOTA();

  if(millis() > rebootTimeout){
    Serial.println("Restart every 6 hours -> Restarting");
    ESP.restart();
  }
  
  delay(1);
}

