#pragma once


Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
bool sensorStatus=false;
uint16_t writePos=0x000;

EEPROM_FLAGS eeprom_state=FLAG_DEF;

void eepromInit() {
  // EEPROM.write(FLAG_ADDR,FLAG_BLANK);
   //EEPROM.write(0x30,0x10);
  
  byte flags = EEPROM.read(FLAG_ADDR);


  if (flags==FLAG_BLANK) {// Buffer is in a blank state:
    // clear whole buffer
    for(uint16_t i=0;i<EEPROM.length();i++){
      EEPROM.write(i,0x00);
    }
    // write first write flag to address 0x000
    EEPROM.write(FLAG_ADDR,FLAG_FIRST_WRITE);
    eeprom_state=FLAG_FIRST_WRITE;
    // set writepos to next address
    writePos=DATA_ADDR;
    
  } else if (flags==FLAG_FIRST_WRITE||flags==FLAG_ADDITIONAL_WRITE) {// Buffer has been written to previously, but not terminated correctly
    EEPROM.write(FLAG_ADDR,FLAG_ADDITIONAL_WRITE);
    Serial.println("Additional write.");
    eeprom_state=FLAG_ADDITIONAL_WRITE;
    // start reading from the back and search for first byte !=0x00
    uint16_t val=0xFF;
    for (writePos=0x398;writePos>0;writePos-=sizeof(uint16_t)){
      EEPROM.get(writePos,val);
      if (val!=0x00&&writePos>PRESS_ADDR+sizeof(float)){//When found
        // set write position to next address
        writePos+=sizeof(uint16_t);
        // write code 0xF0 (240)
        EEPROM.put(writePos,0xF00F);
        writePos+=sizeof(uint16_t);
        break;
      } else if(writePos<=PRESS_ADDR+sizeof(float)){//If it sees the entire eeprom is clean, restart the cycle. 
        Serial.println("Detected previous write, yet there's no data in eeprom. Proceeding as if it was clean.");      
        EEPROM.write(FLAG_ADDR,FLAG_FIRST_WRITE);
        eeprom_state=FLAG_FIRST_WRITE;
        writePos=DATA_ADDR;
        break;
      }
    }
    // move writepos to next address
    
    Serial.print("Write Address: ");
    Serial.println(writePos);
    
  } else if (flags==FLAG_COMPLETED){// Buffer has been written to and terminated successfully
    Serial.println("EEPROM has been written to successfully and hasn't yest been unloaded. Aborting.");
    while(true) {
      digitalWrite(buzzerPin, HIGH);
      delay(900);
      digitalWrite(buzzerPin, LOW);
      delay(100);
    }
  }
}

float baseLevelPressure=1023.25f;

void bmp180Init(){
  Serial.println("Pressure Sensor Test");
  sensorStatus=bmp.begin();
  
  if(!sensorStatus){
    Serial.println("BMP180 Sensor not detected. Aborting.");
    uint16_t shortt=50;
    while(true){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(shortt);
      digitalWrite(LED_BUILTIN, LOW);
      delay(shortt);
    }
    
  } else {
    Serial.print("BMP180 Sensor detected. EEPROM flag:");
    Serial. println(eeprom_state);
    if (eeprom_state==FLAG_FIRST_WRITE) {
      sensors_event_t event;
      bmp.getEvent(&event);
      while (event.type!=SENSOR_TYPE_PRESSURE) {
        bmp.getEvent(&event);
      }
      Serial.println("Pressure event detected.");
      
      baseLevelPressure=event.pressure;
      EEPROM.put(PRESS_ADDR,baseLevelPressure);
      Serial.print("Pressure:  ");
      Serial.println(baseLevelPressure);
      
    } else if (eeprom_state==FLAG_ADDITIONAL_WRITE) {
      Serial.println("Getting pressure value from eeprom");
      EEPROM.get(PRESS_ADDR,baseLevelPressure);
      Serial.print("Pressure:  ");
      Serial.println(baseLevelPressure);
    }
  }
}

void actuatorInit(){
  pinMode(droguePin,OUTPUT);
  digitalWrite(droguePin,LOW);
  pinMode(mainChutePin,OUTPUT);
  digitalWrite(mainChutePin,LOW);
  
  pinMode(buzzerPin,OUTPUT);
  digitalWrite(buzzerPin,LOW);
 
}

