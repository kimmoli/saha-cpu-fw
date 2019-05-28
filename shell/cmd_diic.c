#include <stdlib.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"

void cmd_diic(BaseSequentialStream *chp, int argc, char *argv[])
{
    uint8_t addr;
    uint8_t regaddr;
    int count, i;
    uint8_t rxBuf[16] = {0};
    msg_t ret = MSG_OK;

    if (argc != 3)
    {
        chprintf(chp, "diic deviceaddress regaddress count\n\r");
        return;
    }

    addr = strtol(argv[0], NULL, 16);
    regaddr = strtol(argv[1], NULL, 16);
    count = strtol(argv[2], NULL, 10);

    if (count > 16)
        count = 16;

    i2cAcquireBus(&I2CD1);
    for (i=0; i<count; i++)
    {
        i2cMasterTransmit(&I2CD1, addr, (uint8_t[]){regaddr+i}, 1, NULL, 0);
        ret |= i2cMasterReceive(&I2CD1, addr, rxBuf+i, 1);
    }
    i2cReleaseBus(&I2CD1);

    if (ret != MSG_OK)
    {
        chprintf(chp, "[%02x] Error %d\n\r", addr, ret);
        return;
    }

    chprintf(chp, "[%02x] %02x:", addr, regaddr);
    for (i=0 ; i<count; i++)
        chprintf(chp, " %02x", rxBuf[i]);
    chprintf(chp, "\n\r");
}
