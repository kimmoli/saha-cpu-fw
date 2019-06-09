#ifndef KEYSCAN_H
#define KEYSCAN_H

struct KeyboardDriver
{
    bool active;
    uint8_t i2cAddress;
};

typedef struct KeyboardDriver KeyboardDriver;

extern KeyboardDriver keyboards[8];

extern void initKeyboard(void);

#endif
