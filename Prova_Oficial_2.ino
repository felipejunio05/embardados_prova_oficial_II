#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <OneWire.h>
#include <DallasTemperature.h>

const char auth[] = "4V9-V7ESHHAml2T50tWR17lFQFe6Gv2p";

const char ssid[] = "Matheus";
const char pass[] = "12345678";

const short int SensorTemp = 0;
const short int LedRed = 4;
const short int LedGreen = 5;
const short int Buzzer = 13;

short int PortV00 = 0;
short int PortV01 = 0;
short int PortV04 = 0;

unsigned long int Prev[3] = {0, 0, 0};
unsigned long int Curr = 0;
unsigned long int Interval = 1000;

bool TimeGet = true;
bool Ctrl = true;
bool First = true;
bool Interrupt = false;

OneWire oneWire(SensorTemp);
DallasTemperature sensors(&oneWire);

WidgetLED VirtualLed(V2);

BLYNK_WRITE(V0) {
  PortV00 = param.asInt();
}

BLYNK_WRITE(V1) {
  PortV01 = param.asInt();
}

BLYNK_WRITE(V4) {
  PortV04 = param.asInt();  
}

void setup() {
  pinMode(Buzzer, OUTPUT);
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);

  sensors.begin();
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  
  DisAllComp();
}

void loop() {
  Curr = millis();

  if ( PortV00 && !Interrupt ) {
    if( First ) {
      Prev[2] = Curr;
      First = false;
    }

    setInterval();
    
    if ( Curr - Prev[0] >= Interval ) {
     LedColor(); Buzz();
 
     Prev[0] = Curr;
    }
    
    if ( PortV04 )
     Interrupt = true;
 
    TimeGet = false;
  }

  else {
    DisAllComp();
    First = true; Interval = 1000;

    if ( PortV00 == 0 )
      Interrupt = false;

    if ( !TimeGet )
      TimeGet = true;
  }

  if ( Curr - Prev[1] >= 5000  && TimeGet ) {
    sensors.requestTemperatures();
    Blynk.virtualWrite(V3,(int)sensors.getTempCByIndex(0));

    Prev[1] = Curr;
  }

  Blynk.run();
}

void LedColor() {
  if ( ! VirtualLed.getValue() ) 
    VirtualLed.on();

  if ( digitalRead(LedGreen) == HIGH && digitalRead(LedRed) == LOW)   
    digitalWrite(LedGreen, !digitalRead(LedGreen));

  if ( digitalRead(LedRed) == LOW ) {
    digitalWrite(LedRed, !digitalRead(LedRed));
    VirtualLed.setColor("#D3435C");
  }
  else {
    digitalWrite(LedRed, !digitalRead(LedRed));
    digitalWrite(LedGreen, !digitalRead(LedGreen));

    VirtualLed.setColor("#23C48E");
  }
}

void DisAllComp() {
  digitalWrite(LedRed,LOW);
  digitalWrite(LedGreen,LOW);

  VirtualLed.off();
  analogWrite(Buzzer,0);
}

void Buzz() {
  if ( Ctrl ) {
    if ( PortV01 == 0 )
      analogWrite(Buzzer,90);
    else
      analogWrite(Buzzer,PortV01);

    Ctrl = false;
  }

  else {
    analogWrite(Buzzer,0);
    Ctrl = true;
  }
}

void setInterval() {
  if(Curr - Prev[2] > 10000 && Curr - Prev[2] < 20000) {
    Interval = 600;
  }
  
  else if( Curr - Prev[2] > 20000 && Curr - Prev[2] < 40000 ) {
    Interval = 300;
  }
  
  else if( Curr - Prev[2] > 40000 ) {
    Interval = 200;
  }
}
