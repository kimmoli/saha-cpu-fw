#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "rele.h"

void cmd_rele(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc == 1)
    {
        setRelayRaw(strtol(argv[0], NULL, 10));
        return;
    }

    chprintf(chp, "Relays %d\n\r", getRelay());
    chprintf(chp, "Inputs %d\n\r", getRelayInputs());
}

