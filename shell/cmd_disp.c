#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "display.h"

void cmd_disp(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) chp;

    if (argc == 1)
    {
        for (int i=0 ; i<8; i++)
        {
            if (displays[i].active && strlen(argv[0]) > i*4)
            {
                int d = 0;
                for (int c = i*4 ; c < strlen(argv[0]) ; c++)
                {
                    displays[i].digits[d++] = argv[0][c];
                }
            }
        }
        displayUpdate();
    }
}

