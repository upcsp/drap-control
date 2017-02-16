#include <EEPROM.h>
#include <Adafruit_BMP085_U.h>

#include "moduleInit.h"

#include "bmp180.h"

const uint8_t bufferSize=32;
uint16_t dataBuffer[bufferSize];

void waitForMovement(){
  uint16_t pos=0;
  uint16_t altitude=0;
  while(altitude<5){
    Serial.print("Within margin:  ");
    Serial.println(altitude);
    dataBuffer[pos%bufferSize]=altitude;
    delay(200);
    altitude=getHeight();
  }
  Serial.println("Movement detected.");
  
  for(int i=0;i<32;i++){
    EEPROM.put(writePos,dataBuffer[(pos-bufferSize+i+1)%bufferSize]);
    writePos+=sizeof(uint16_t);
  }
  
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  // Initialize EEPROM
  eepromInit();
  
  // Initialize atmospheric sensor and height  
  bpm180Init();
  
  // Initialize actuators
  actuatorInit();

  waitForMovement();

}

void loop() {
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);

}
