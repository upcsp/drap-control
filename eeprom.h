#pragma once

const uint16_t FLAG_ADDR=0x000;
const uint16_t PRESS_ADDR=0x002;
const uint16_t DATA_ADDR=PRESS_ADDR+sizeof(float);

enum EEPROM_FLAGS : uint8_t {
  FLAG_BLANK=0x00,
  FLAG_DEF=0xA0,
  FLAG_FIRST_WRITE=0xA1,
  FLAG_ADDITIONAL_WRITE=0xA2,
  FLAG_COMPLETED=0xFF
};
