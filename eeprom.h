#ifndef _EEPROM_H
#define _EEPROM_H

#define EEPROM_ADDR 0x50

extern uint16_t getEeprom(uint16_t addr);
extern void setEeprom(uint16_t addr, uint16_t value);

#endif
