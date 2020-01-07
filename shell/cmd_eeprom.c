#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "eeprom.h"

void cmd_eeprom(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc == 2)
    {
        setEeprom(strtol(argv[0], NULL, 10), strtol(argv[1], NULL, 10));
        return;
    }
    else if (argc == 1)
    {
        chprintf(chp, "%d\n\r", getEeprom(strtol(argv[0], NULL, 10)));
    }
    else
    {
        chprintf(chp, "ee addr {value}\n\r");
    }
}

