#ifndef _RELE_H
#define _RELE_H

#define RELE_ADDR 0x18

#define RELAY_1 0x10
#define RELAY_2 0x20
#define RELAY_3 0x40
#define RELAY_4 0x80

extern void setRelay(uint8_t rele, bool value);
extern void setRelayRaw(uint8_t value);
extern uint8_t getRelay(void);
extern uint16_t getRelayInputs(void);

#endif
