#include "hal.h"
#include "i2c.h"
#include "helpers.h"
#include "rele.h"

void setRele(uint8_t value)
{
    uint8_t txbuf[2] = {0};

    i2cAcquireBus(&I2CD1);
    txbuf[0] = 0x03;
    txbuf[1] = 0x0f;
    i2cMasterTransmit(&I2CD1, RELE_ADDR, txbuf, 2, NULL, 0);
    txbuf[0] = 0x01;
    txbuf[1] = (value << 4) & 0xf0;
    i2cMasterTransmit(&I2CD1, RELE_ADDR, txbuf, 2, NULL, 0);
    i2cReleaseBus(&I2CD1);
}

uint8_t getRele(void)
{
    uint8_t txbuf[1] = {0};
    uint8_t rxbuf[1] = {0};

    i2cAcquireBus(&I2CD1);
    txbuf[0] = 0x01;
    i2cMasterTransmit(&I2CD1, RELE_ADDR, txbuf, 1, rxbuf, 1);
    i2cReleaseBus(&I2CD1);

    return (rxbuf[0] & 0xf0) >> 4;
}
