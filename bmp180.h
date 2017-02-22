#pragma once

extern Adafruit_BMP085_Unified bmp;
extern float baseLevelPressure;

float getPressure(){
	sensors_event_t event;
	do {
		bmp.getEvent(&event);
	}while (event.type!=SENSOR_TYPE_PRESSURE);

  return event.pressure;
}

float getTemperature(){
  float temp;
  bmp.getTemperature(&temp);
  return temp;
}

float getAltitude(){
  return bmp.pressureToAltitude(baseLevelPressure,getPressure(),getTemperature());
}
