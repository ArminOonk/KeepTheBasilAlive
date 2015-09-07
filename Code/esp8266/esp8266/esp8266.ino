#include <OneWire.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h>

OneWire  ds(2);  // on pin D4 LoLin board (a 4.7K resistor is necessary)

const char *ssid = "tv";
const char *password = "AAECUPUK";

byte addr[8];
bool type_s = false;
bool ds18b20Found = false;
float getTemperature()
{
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  byte data[12];
  for (int i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  int16_t raw = (data[1] << 8) | data[0];

  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  float celsius = (float)raw / 16.0;

  return celsius;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Get ds18b20 temperature sensor
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
  }

  Serial.print("ROM =");
  for(int i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = true;
      ds18b20Found = true;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = false;
      ds18b20Found = true;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = false;
      ds18b20Found = true;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  }
}

int value = 0;

void loop() {
  if(ds18b20Found) {
    float temperature = getTemperature();
    Serial.println("Temperature = " + String(temperature));
  }
  Serial.println("Count: " + String(value));
  value++;
  delay(1000);
}
