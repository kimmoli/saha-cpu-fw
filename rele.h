#ifndef _RELE_H
#define _RELE_H

#define RELE_ADDR 0x18

#define RELAY_1 0x10
#define RELAY_2 0x20
#define RELAY_3 0x40
#define RELAY_4 0x80

#define RELE_MASK 0xF0

#define AUX_IN_INVERT 0x0B00

#define AUX_IN_1     0x0100
#define AUX_IN_2     0x0200
#define AUX_IN_3     0x0400
#define AUX_IN_4     0x0800

extern void setRelay(uint8_t rele, bool value);
extern void setRelayRaw(uint8_t value);
extern uint8_t getRelay(void);
extern uint16_t getRelayInputs(void);

#endif
