#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "private.h"

OneWire  ds(5);  // on pin D4 LoLin board (a 4.7K resistor is necessary)

byte addr[8];
bool type_s = false;
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
  Serial.println("                    ");
  Serial.println("                    ");
  delay(10);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(10);
  
  Serial.println("ESP ID: " + String(ESP.getChipId()));
  delay(10);
  Serial.println("sizeof(int): " + String(sizeof(int)));
  delay(10);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  initTemperature();

  ESP.wdtEnable(0);
  Wire.begin(2, 14);
  resetChirp(); //reset
}

int value = 0;
long nextTimeReport = 0;
long lastTimeReport = 0;
void SendThingspeak(float temperature){
  Serial.print("connecting to ");
  Serial.println(host);
    
  WiFiClient client;
  const int httpPort = 80;
  if (client.connect(host, httpPort)) 
  {
    String url = GET + String(temperature);
    url += "&field2=" + String(0.001*millis());
    Serial.print("Requesting URL: ");
    Serial.println(url);

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
          Serial.println("Data received");
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
    Serial.println("connection failed");
  }
}

int lastLoopTime = 0;
const int loopTimeout = 1000;

void loop() {
  if(ds18b20Found) {
    float temperature = getTemperature();
    yield();

    unsigned int chirpCapacitance = readCapacitance();
    yield();
    unsigned int chirpTemperature = readChirpTemperature();
    yield();
    unsigned int chirpLight = readLight();
    yield();

    Serial.print("Chirp> ");
    Serial.print("Capacitance: " + String(chirpCapacitance) + " ");
    Serial.print("Temperature: " + String(chirpTemperature) + " ");
    Serial.println("Light: " + String(chirpLight));
    
    if(millis() > nextTimeReport)
    {
      if(millis() > rebootTimeout){
        Serial.println("Restart every 6 hours -> Restarting");
        ESP.restart();
      }
      
      if(WiFi.status() != WL_CONNECTED) {
        Serial.println("Wifi not connected -> Restarting");
        ESP.restart();
      }
      
      SendThingspeak(temperature);
    }
    Serial.println(String(value) + ": Temperature = " + String(temperature));
  
  } else {
    Serial.println("Count: " + String(value));
  }
  value++;

  if(millis() > (lastTimeReport + 2*updateTimeout)) {
    Serial.println("No server response -> Restarting");
    ESP.restart();
  }

  int dt = (lastLoopTime+loopTimeout)-millis();
  if(dt <= 0) {
    dt = 1;
  }
  lastLoopTime = millis();
  delay(dt);
}

