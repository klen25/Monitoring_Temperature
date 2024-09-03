#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

#define pinPBOn 7
#define pinPBVault 8
#define pinLEDRun 10
#define pinLEDVault 11
#define pinLEDOverHeat 12
#define pinValve 6

#define OverHeat 70.0

unsigned long lastmillis = 0;
float Suhu = 0.0;
bool isOverHeat = false, isonpressed = false, isvaultpressed = false, CekOverHeat = true;

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  
  pinMode(pinPBOn, INPUT);
  pinMode(pinPBVault, INPUT);
  pinMode(pinLEDRun, OUTPUT);
  pinMode(pinLEDVault, OUTPUT);
  pinMode(pinLEDOverHeat, OUTPUT);
  pinMode(pinValve, OUTPUT);

  digitalWrite(pinLEDRun, HIGH);
  digitalWrite(pinLEDVault, HIGH);
  digitalWrite(pinLEDOverHeat, HIGH);
  
  digitalWrite(pinValve, HIGH);
  
  isonpressed = false;
  isvaultpressed = false;
  isOverHeat = false;
  CekOverHeat = true;
}

void loop() {
  CekTombol();
  if(isonpressed) {
    if(millis() - lastmillis >= 1000) {
      Suhu = readDHT();
      Serial.print("Suhu: ");Serial.println(Suhu);
      if(Suhu >= OverHeat) {
        if(!isOverHeat && CekOverHeat) {
          CekOverHeat = false;
          isOverHeat = true;
          Serial.println("Vault, OverHeat, Valve Active");
          digitalWrite(pinLEDVault, LOW);
          digitalWrite(pinLEDOverHeat, LOW);
          digitalWrite(pinValve, LOW);
        }
          if(isvaultpressed) {
            isvaultpressed = false;
            Serial.println("Vault Off");
            digitalWrite(pinLEDVault, HIGH);
            digitalWrite(pinLEDOverHeat, HIGH);
            digitalWrite(pinValve, HIGH);
          }
      }
      else {
        CekOverHeat = true;
        if(isOverHeat) {
          if(isvaultpressed) {
            isvaultpressed = false;
            Serial.println("Vault Off");
            digitalWrite(pinLEDVault, HIGH);
            digitalWrite(pinLEDOverHeat, HIGH);
            digitalWrite(pinValve, HIGH);
          }
        }
        else {
          if(isOverHeat) {
            isOverHeat = false;
            Serial.println("Valve Off");
            digitalWrite(pinLEDOverHeat, HIGH);
            digitalWrite(pinValve, HIGH);
          }
        }
      }
    }    
  }
}

uint8_t ONState, LastONState = 0;
uint8_t VaultState, LastVaultState = 0;
void CekTombol() {
  ONState = digitalRead(pinPBOn);
  VaultState = digitalRead(pinPBVault);

  if(ONState != LastONState) {
    if(ONState == LOW) {
      isonpressed = true;
      Serial.println("Run");
      digitalWrite(pinLEDRun, LOW);
    }
  }
  LastONState = ONState;

  if(VaultState != LastVaultState && isOverHeat) {
    if(VaultState == LOW) {
      isvaultpressed = true;
    }
  }
  LastVaultState = VaultState;
}

float readDHT() {
  float t = dht.readTemperature();
  if (isnan(t)) { t = 0.0; }

//Serial.printf("Temperature: %0.2f °C\r\n", t);
  return t;
}
