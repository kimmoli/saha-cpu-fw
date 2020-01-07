#include "hal.h"
#include "i2c.h"
#include "helpers.h"
#include "eeprom.h"

void setEeprom(uint16_t addr, uint16_t value)
{
    uint8_t txbuf[4] = {0};

    i2cAcquireBus(&I2CD1);
    txbuf[0] = ((2*addr) & 0xff00) >> 8;
    txbuf[1] = (2*addr) & 0xff;
    txbuf[2] = (value & 0xff00) >> 8;
    txbuf[3] = value & 0xff;
    i2cMasterTransmit(&I2CD1, EEPROM_ADDR, txbuf, 4, NULL, 0);
    i2cReleaseBus(&I2CD1);
}

uint16_t getEeprom(uint16_t addr)
{
    uint8_t txbuf[2] = {0};
    uint8_t rxbuf[2] = {0};

    i2cAcquireBus(&I2CD1);
    txbuf[0] = ((2*addr) & 0xff00) >> 8;
    txbuf[1] = (2*addr) & 0xff;
    i2cMasterTransmit(&I2CD1, EEPROM_ADDR, txbuf, 2, rxbuf, 2);
    i2cReleaseBus(&I2CD1);

    return (rxbuf[0] << 8 )| rxbuf[1];
}

