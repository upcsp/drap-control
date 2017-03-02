#pragma once

enum ACTUATOR_STATE : uint8_t {
  STATE_DISENGAGED,
  STATE_ARMED,
  STATE_ENGAGED,
  STATE_EXPIRED
};

enum ACTUATOR_TYPE : uint8_t {
  TYPE_DROGUE=0,
  TYPE_MAIN_CHUTE=1
};


ACTUATOR_STATE drogueState=STATE_DISENGAGED;
const uint8_t droguePin=2;
ACTUATOR_STATE mainChuteState=STATE_DISENGAGED;
const uint8_t mainChutePin=3;

const uint16_t drogueBuffSize=32;

const uint8_t buzzerPin=4;
const uint8_t switchSource=9;
const uint8_t switchPin=10;

void checkDrogueState(uint16_t altitude){
  const static uint32_t activationDuration=2000;//in millis
  
  static uint8_t pos=0;
  static uint16_t drogueBuff[drogueBuffSize]={0};
  static uint32_t activationTime=0;

  Serial.print("Checking state of drogue. State is:");
  switch (drogueState){
    case STATE_DISENGAGED:
    Serial.println("STATE_DISENGAGED");
      if(altitude>8){
        Serial.println("Arming drogue.");
        drogueState=STATE_ARMED;
      }
      break;
    case STATE_ARMED:
    Serial.println("STATE_ARMED");
      drogueBuff[pos%drogueBuffSize]=altitude;
      pos++;
      if(drogueBuff[pos%drogueBuffSize]>(altitude)&&drogueBuff[(pos+1)%drogueBuffSize]>(altitude)&&drogueBuff[(pos+2)%drogueBuffSize]>(altitude)){
        Serial.println("Engaging drogue.");
        drogueState=STATE_ENGAGED;
        activationTime=millis();
        //enable pin
        digitalWrite(droguePin,HIGH);
      }
      break;
    case STATE_ENGAGED:
    Serial.println("STATE_ENGAGED");
      
      if(activationTime+activationDuration<millis()){
        Serial.println("Drogue disengaged");
        drogueState=STATE_EXPIRED;
        // disable pin
        digitalWrite(droguePin,LOW);
        digitalWrite(buzzerPin,HIGH);
      }

      break;
    case STATE_EXPIRED:
    Serial.println("STATE_EXPIRED");

      break;
  }
}

void checkMainChuteState(uint16_t altitude){
  const static uint32_t activationDuration=2000;//in millis

  static uint32_t activationTime=0;


  Serial.print("Checking main chute state. State is:");
  switch (mainChuteState){
    case STATE_DISENGAGED:
    Serial.println("STATE_DISENGAGED");
      if(drogueState==STATE_EXPIRED){//Once drogue is deployed
        Serial.println("Arming main chute.");
        mainChuteState=STATE_ARMED;
      }
      break;
    case STATE_ARMED:
    Serial.println("STATE_ARMED");
      if(altitude<7){//TODO:Find a proper altitude to activate this
        Serial.println("Engaging main chute.");
        mainChuteState=STATE_ENGAGED;
        activationTime=millis();
        //enable pin
        digitalWrite(mainChutePin,HIGH);
      }
      break;
    case STATE_ENGAGED:
    Serial.println("STATE_ENGAGED");
      if(activationTime+activationDuration<millis()){
        
        Serial.println("Disengaging main chute.");
        mainChuteState=STATE_EXPIRED;
        // disable pin
        digitalWrite(mainChutePin,LOW);
      }
      break;
    case STATE_EXPIRED:
    Serial.println("STATE_EXPIRED");

      break;
  }
}

void checkStates(uint16_t altitude){
  checkDrogueState(altitude);
  checkMainChuteState(altitude);
}



