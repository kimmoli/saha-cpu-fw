#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "stepper.h"

void cmd_step(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc == 2)
    {
        step(&STEPPERD1, strtol(argv[0], NULL, 10));
        step(&STEPPERD2, strtol(argv[1], NULL, 10));
    }
}

