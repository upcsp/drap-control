#include <EEPROM.h>
#include <Adafruit_BMP085_U.h>

#include "actuators.h"
#include "eeprom.h"

#include "bmp180.h"

#include "moduleInit.h"

uint16_t timeStep=100;

const uint8_t bufferSize=32;
uint16_t dataBuffer[bufferSize];

uint32_t loopMil=millis();

void waitForMovement(){
  uint16_t pos=0;
  uint16_t altitude=0;
  uint32_t mil=millis();
  bool buzzerState=false;
  while(altitude<10){
    Serial.print("Within margin:  ");
    Serial.println(altitude);
    dataBuffer[pos%bufferSize]=altitude;
    if((uint32_t(millis()/500)-uint32_t(mil/500))>0){
      buzzerState= !buzzerState;
    }
    digitalWrite(buzzerPin,buzzerState);
    mil=millis();
    delay(timeStep);
    altitude=getAltitude();
    ++pos;
  }

  digitalWrite(buzzerPin,LOW);
  Serial.print("Movement detected.");
  Serial.print("Last pos:");
  Serial.println(pos);
  
  for(int i=0;i<bufferSize;i++){
    uint16_t bufferPos=(pos-bufferSize+i)%bufferSize;
    Serial.print("BufferPos:");
    Serial.println(bufferPos);
    Serial.print("WritePos:");
    Serial.println(writePos);
    Serial.print("Value:");
    Serial.println(dataBuffer[bufferPos]);
    EEPROM.put(writePos,dataBuffer[bufferPos]);
    writePos+=sizeof(uint16_t);
  }
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(buzzerPin,HIGH);
  // Initialize actuators
  actuatorInit();

  // Initialize EEPROM
  eepromInit();
  
  // Initialize atmospheric sensor and height  
  bmp180Init();
  
  digitalWrite(buzzerPin,LOW);
  
  // If it's already launched it should go straight to the loop
  if(eeprom_state==FLAG_FIRST_WRITE){
    waitForMovement();
  }

  loopMil=millis();
}

void loop() {
  uint16_t altitude = getAltitude();
  Serial.println(altitude);

  // Write data to eeprom
  EEPROM.put(writePos,altitude);
  writePos+=sizeof(uint16_t);


  // Check state of actuators
  checkStates(altitude);

  // Check ending condition
  if((altitude<5&&mainChuteState==STATE_EXPIRED&&drogueState==STATE_EXPIRED)||writePos>=EEPROM.length()){
    //Successfully terminate EEPROM
    EEPROM.write(FLAG_ADDR,FLAG_COMPLETED);
    digitalWrite(buzzerPin, HIGH);
    while(true){
      digitalWrite(buzzerPin,HIGH);
      delay(1000);
      digitalWrite(buzzerPin,LOW);
      delay(3000);
      
    }
  }
  
  uint32_t currMil=millis();
  uint32_t buzzState=uint32_t(currMil/200)-uint32_t(loopMil/200);
  if(buzzState==8){
      digitalWrite(buzzerPin,HIGH);
      loopMil=millis();
  }
  if(buzzState==1){
      digitalWrite(buzzerPin,LOW);
  }
  if(buzzState==2){
      digitalWrite(buzzerPin,HIGH);
  }
  if(buzzState==3){
      digitalWrite(buzzerPin,LOW);
  }

  delay(timeStep);

}
