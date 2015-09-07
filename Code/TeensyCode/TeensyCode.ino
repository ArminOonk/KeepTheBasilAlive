int OnboardLed = 13;
int ButtonLed = 3;
int Button = 2;

int Working = 4;
int LowSupplies = 9;
int Thirsty = 6;

int PumpPin = 10;
unsigned PumpTimeout = 60*60*1000; // 1 Hour
unsigned PumpInitTime = 5*60*1000;
bool FirstPump = true;
unsigned PumpTime = 10*1000; // Pump for 10 seconds
bool isPumpOn = false;
bool PumpManual = false;


// Higher is wetter
int StartPump = 11000;
unsigned LastPumpTime = 0;

void PumpOn()
{
  if(!isPumpOn)
  {
    Serial.println("Pumping...");
    LastPumpTime = millis(); // First time we turn the pump on, save time
     
    analogWrite(Working, 255);
    isPumpOn = true;
     
    digitalWrite(PumpPin, HIGH); 
  }
}

void PumpOff()
{
  digitalWrite(PumpPin, LOW); 
  analogWrite(Working, 0);
  isPumpOn = false;
}

bool isThirsty = false;
void CheckMoisture()
{
  //int moist = analogRead(A0);
  int moist = touchRead(16);
  Serial.println("Moisture: " + String(moist) + " trigger @<: " + String(StartPump));
  
  isThirsty = (moist < StartPump);
  
  // Do not start pumping the first 5 minutes
  if((millis() > PumpInitTime))
  {
    if(FirstPump && isThirsty)
    {
      FirstPump = false;
      PumpOn();
    }
    
    if(millis() > (LastPumpTime + PumpTimeout) )
    {
      if(isThirsty)
      {
        // Start pump
        PumpOn();
      }
      else
      {
        LastPumpTime = millis();
      }
    }
  }  
}


// the setup routine runs once when you press reset:
void setup() 
{                
  // initialize the digital pin as an output.
  pinMode(OnboardLed, OUTPUT);   
  pinMode(PumpPin, OUTPUT);

  // Button
  pinMode(Button, INPUT_PULLUP);

  Serial.begin(115200);
}

bool advertise = true;

// Start Led Button fading
#define COR_LENGTH 16
char fadeCor[COR_LENGTH] = {0, 1, 2, 3, 4, 6, 8, 12, 16, 23, 32, 45, 64, 90, 128, 180};
int fadeCnt = 0;
void loop() 
{ 
  if(millis() > 3000 && advertise)
  {
    Serial.println("Starting KeepTheBasilAlive");
    advertise = false;
  }
  
  if (digitalRead(Button))
  {
    CheckMoisture();
    
    if((isPumpOn && (millis() > (LastPumpTime + PumpTime))) || PumpManual)
    {
      PumpOff(); // long enough
      PumpManual = false;
    }
    
    // Button Led fading (Show alive)
    digitalWrite(OnboardLed, HIGH);
    bool showFade = (fadeCnt++%5 == 0);

    for (int fadeValue = 0 ; fadeValue < COR_LENGTH; fadeValue++) 
    {
      if(showFade)
      {
        analogWrite(ButtonLed, fadeCor[fadeValue]);
      }
      if(isThirsty)
      {
        analogWrite(Thirsty, fadeCor[fadeValue]);
      }
      delay(40);
    }

    digitalWrite(OnboardLed, LOW);
    for (int fadeValue = COR_LENGTH-1 ; fadeValue >= 0; fadeValue--) 
    {
      if(showFade)
      {
        analogWrite(ButtonLed, fadeCor[fadeValue]);
      }
      if(isThirsty)
      {
        analogWrite(Thirsty, fadeCor[fadeValue]);
      }
      delay(40);
    }

  }
  else
  {
    // Manual
    PumpManual = true;
    analogWrite(ButtonLed, fadeCor[COR_LENGTH-1]);
    digitalWrite(OnboardLed, HIGH);
    PumpOn();
  }
  
  
}
