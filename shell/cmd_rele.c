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
        setRele(strtol(argv[0], NULL, 10));
        return;
    }

    chprintf(chp, "%d\n\r", getRele());
}

