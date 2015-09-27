int OnboardLed = 13;

#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 50

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 17

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
  delay(3000); // sanity delay

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.showColor(CHSV(50, 255, 255));
  FastLED.setCorrection(TypicalSMD5050);
  FastLED.setBrightness( 75 );
}

void loop() {   
  static uint8_t brightness;
  FastLED.setBrightness( brightness );
  
  if(brightness > 127)
  {
    // End White
    FastLED.showColor(CHSV(50, 255-2*brightness, 255));
  }
  else
  {
    // Start Yellow
    FastLED.showColor(CHSV(brightness/2, 255, 255));
  }
  brightness++;
  delay(100);
}
