int OnboardLed = 13;
int ButtonLed = 3;
int Button = 2;

int Working = 4;
int LowSupplies = 9;
int Thirsty = 6;

// the setup routine runs once when you press reset:
void setup() 
{                
  // initialize the digital pin as an output.
  pinMode(OnboardLed, OUTPUT);   

  // Button
  pinMode(Button, INPUT_PULLUP);

  Serial.begin(115200);
  
}

bool advertise = true;

// Start Led Button fading
#define COR_LENGTH 16
char fadeCor[COR_LENGTH] = {0, 1, 2, 3, 4, 6, 8, 12, 16, 23, 32, 45, 64, 90, 128, 180};

void loop() 
{ 
  if(millis() > 3000 && advertise)
  {
    Serial.println("Starting KeepTheBasilAlive");
    advertise = false;
  }
  
  if (digitalRead(Button))
  {
    // Button Led fading (Show alive)
    digitalWrite(OnboardLed, HIGH);
    for (int fadeValue = 0 ; fadeValue < COR_LENGTH; fadeValue++) 
    {
      analogWrite(ButtonLed, fadeCor[fadeValue]);
      analogWrite(Working, fadeCor[fadeValue]);
      analogWrite(LowSupplies, fadeCor[fadeValue]);
      analogWrite(Thirsty, fadeCor[fadeValue]);
      delay(40);
    }
  
    // fade out from max to min in increments of 5 points:
    digitalWrite(OnboardLed, LOW);
    for (int fadeValue = COR_LENGTH-1 ; fadeValue >= 0; fadeValue--) 
    {
      analogWrite(ButtonLed, fadeCor[fadeValue]);
      analogWrite(Working, fadeCor[fadeValue]);
      analogWrite(LowSupplies, fadeCor[fadeValue]);
      analogWrite(Thirsty, fadeCor[fadeValue]);
      delay(40);
    }
  }
  else
  {
    // Pumping?
    analogWrite(ButtonLed, fadeCor[COR_LENGTH-1]);
    digitalWrite(OnboardLed, HIGH);
  }
}
