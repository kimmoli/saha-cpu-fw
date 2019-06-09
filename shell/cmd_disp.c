#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "display.h"
#include "helpers.h"

void cmd_disp(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc == 1)
    {
        for (int i=0 ; i<8; i++)
        {
            if (displays[i].active && strlen(argv[0]) > i*4)
            {
                int d = 0;
                for (int c = i*4 ; c < MIN(i*4+4, strlen(argv[0])) ; c++)
                {
                    displays[i].digits[d++] = argv[0][c];
                }

                chprintf(chp, "%d: %c%c%c%c\n\r", i, displays[i].digits[0] & 0x7f,
                                                     displays[i].digits[1] & 0x7f,
                                                     displays[i].digits[2] & 0x7f,
                                                     displays[i].digits[3] & 0x7f );
            }
        }
        displayUpdate();
    }
    else
    {
        for (int i=0 ; i<8; i++)
        {
            if (displays[i].active)
            {
                chprintf(chp, "%d: %c%c%c%c\n\r", i, displays[i].digits[0] & 0x7f,
                                                     displays[i].digits[1] & 0x7f,
                                                     displays[i].digits[2] & 0x7f,
                                                     displays[i].digits[3] & 0x7f );
            }
        }
    }
}

