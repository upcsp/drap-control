#include <EEPROM.h>
#include <Adafruit_BMP085_U.h>

#include "eeprom.h"

#include "bmp180.h"

#include "actuators.h"


#include "moduleInit.h"

uint16_t timeStep=100;

const uint8_t bufferSize=32;
uint16_t dataBuffer[bufferSize];

void waitForMovement(){
  uint16_t pos=0;
  uint16_t altitude=0;
  while(altitude<5){
    Serial.print("Within margin:  ");
    Serial.println(altitude);
    dataBuffer[pos%bufferSize]=altitude;
    delay(timeStep);
    altitude=getAltitude();
    ++pos;
  }
  Serial.print("Movement detected.");
  Serial.print("Last pos:");
  Serial.println(pos);
  
  for(int i=0;i<bufferSize;i++){
    uint8_t bufferPos=(pos-bufferSize+i)%bufferSize;
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
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  // Initialize actuators
  actuatorInit();

  // Initialize EEPROM
  eepromInit();
  
  // Initialize atmospheric sensor and height  
  bmp180Init();
  
  

  // If it's already launched it should go straight to the loop
  if(eeprom_state==FLAG_FIRST_WRITE){
    waitForMovement();
  }
}

void loop() {
  uint16_t altitude = getAltitude();
  Serial.println(altitude);
  // Write data to eeprom
  //TODO:Check bounds of writePos
  EEPROM.put(writePos,altitude);
  writePos+=sizeof(uint16_t);


  // Check state of actuators
  checkStates(altitude);

  // Check ending condition
  if(altitude<5&&mainChuteState==STATE_EXPIRED&&drogueState==STATE_EXPIRED||writePos>=EEPROM.length()){
    //Successfully terminate EEPROM
    EEPROM.write(FLAG_ADDR,FLAG_COMPLETED);
    digitalWrite(LED_BUILTIN, HIGH);
    while(true){
      
    }
  }

  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(timeStep/2);
  digitalWrite(LED_BUILTIN, LOW);
  delay(timeStep/2);

}
