#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"

void cmd_in(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    chprintf(chp, "IN 1 : %d\n\r", palReadLine(LINE_IN_1));
    chprintf(chp, "IN 2 : %d\n\r", palReadLine(LINE_IN_2));
    chprintf(chp, "IN 3 : %d\n\r", palReadLine(LINE_IN_3));
    chprintf(chp, "IN 4 : %d\n\r", palReadLine(LINE_IN_4));
}

