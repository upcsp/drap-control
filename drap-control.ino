#include <EEPROM.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
bool sensorStatus=false;
uint16_t writePos=0x000;

const uint16_t FLAG_ADDR=0x000;
enum EEPROM_FLAGS {
  FLAG_BLANK=0x00,
  FLAG_DEFAULT=0xA0,
  FLAG_FIRST_WRITE=0xA1,
  FLAG_ADDITIONAL_WRITE=0xA2,
  FLAG_COMPLETED=0xFF
};

EEPROM_FLAGS eeprom_state=FLAG_DEFAULT;

const uint16_t PRESS_ADDR=0x001;
float baseLevelPressure=1023.25f;


enum ACTUATOR_STATE {
  STATE_DISENGAGED,
  STATE_ARMED,
  STATE_ENGAGED,
  STATE_EXPIRED
};

enum ACTUATOR_TYPE {
  TYPE_DROGUE,
  TYPE_MAIN_CHUTE
};

ACTUATOR_STATE states[2]={STATE_DISENGAGED};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  // Initialize EEPROM
  // read EEPROM, check if there's something written by checking 0x000 
  // then, set write location to rightmost nonzero position
  // EEPROM.write(FLAG_ADDR,FLAG_BLANK);
  byte flags = EEPROM.read(FLAG_ADDR);


  if(flags==FLAG_BLANK){// Buffer is in a blank state:
    // write first write flag to address 0x000
    EEPROM.write(FLAG_ADDR,FLAG_FIRST_WRITE);
    eeprom_state=FLAG_FIRST_WRITE;
    // set writepos to next address
    ++writePos;
    
  } else if(flags==FLAG_FIRST_WRITE) {// Buffer has been written to previously, but not terminated correctly
    EEPROM.write(FLAG_ADDR,FLAG_ADDITIONAL_WRITE);
    eeprom_state=FLAG_ADDITIONAL_WRITE;
    // start reading from the back and search for first byte !=0x00
    for(uint16_t i=0x398;i>PRESS_ADDR+sizeof(float);--i){
      if(EEPROM.read(i)!=0x00){//When found
        // set write position to next address
        writePos=i+1;
        // write code 0xF0 (240)
        EEPROM.write(writePos,0xF0);
        // move writepos to next address
        ++writePos;
        break;
      }
    }
    
  } else if(flags==FLAG_COMPLETED){// Buffer has been written to and terminated successfully
    Serial.println("EEPROM has been written to successfully and hasn't yest been unloaded. Aborting.");
    uint16_t longt=400;
    uint16_t shortt=100;
    while(true){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(shortt);
      digitalWrite(LED_BUILTIN, LOW);
      delay(shortt);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(shortt);
      digitalWrite(LED_BUILTIN, LOW);
      delay(longt);
    }
  }

  
  // Initialize atmospheric sensor and height
  
  Serial.println("Pressure Sensor Test");
  sensorStatus=bmp.begin();
  
  if(!sensorStatus){
    Serial.println("BMP180 Sensor not detected. Aborting.");
    uint16_t longt=400;
    uint16_t shortt=100;
    while(true){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(longt);
      digitalWrite(LED_BUILTIN, LOW);
      delay(longt);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(shortt);
      digitalWrite(LED_BUILTIN, LOW);
      delay(longt);
    }
    
  } else {
    if(eeprom_state==FLAG_FIRST_WRITE){
      sensors_event_t event;
      bmp.getEvent(&event);
      while(!event.pressure){
        bmp.getEvent(&event);
      }
      Serial.println("Pressure event detected.");
      
      baseLevelPressure=event.pressure;
      EEPROM.put(PRESS_ADDR,baseLevelPressure);
      Serial.print("Pressure:  ");
      Serial.println(baseLevelPressure);
      
    } else if(eeprom_state==FLAG_ADDITIONAL_WRITE) {
      Serial.println("Getting pressure value from eeprom");
      EEPROM.get(PRESS_ADDR,baseLevelPressure);
      Serial.print("Pressure:  ");
      Serial.println(baseLevelPressure);
    }
    
  }

  
  
  // Initialize actuators

}

void loop() {
  
  digitalWrite(LED_BUILTIN, HIGH);
  // put your main code here, to run repeatedly:

}
