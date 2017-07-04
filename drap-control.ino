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
	while(altitude<5){
		Serial.print("Within margin:	");
		Serial.println(altitude);
		delay(timeStep);
		altitude=getAltitude();
		EEPROM.put(writePos,dataBuffer[bufferPos]);
		writePos=(writePos+sizeof(uint16_t)-DATA_ADDR)%bufferSize+DATA_ADDR;
	}

	Serial.print("Movement detected.");
	Serial.print("Last pos:");
	Serial.println(pos);

	writePos=DATA_ADDR+bufferSize*sizeof(uint16_t);
}

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);

	// Initialize EEPROM
	eepromInit();

	// Initialize atmospheric sensor and height	
	bmp180Init();

	// Initialize actuators
	actuatorInit();

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
	if(altitude<5&&mainChuteState==STATE_EXPIRED&&drogueState==STATE_EXPIRED||writePos==EEPROM.length()){
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
