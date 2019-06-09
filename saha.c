#include "hal.h"
#include <stdio.h>
#include "keyscan.h"
#include "saha.h"
#include "display.h"
#include "helpers.h"

virtual_timer_t sahaVt;

static void displayHandler(char *upper, char *lower)
{
    displays[0].digits[0] = upper[0];
    displays[0].digits[1] = upper[1];
    displays[0].digits[2] = upper[2];
    displays[0].digits[3] = upper[3];

    displays[1].digits[0] = lower[0];
    displays[1].digits[1] = lower[1];
    displays[1].digits[2] = lower[2];
    displays[1].digits[3] = lower[3];

    updateDisplay();
}

static THD_FUNCTION(sahaThread, arg)
{
    (void)arg;

    char upper[5];
    char lower[5];

    uint16_t pgmval = 0;
    uint16_t absval = 0;

    event_listener_t elKbd;

    chEvtRegister(&keyboardEvent, &elKbd, 0);

    while (!chThdShouldTerminateX())
    {
        if (chEvtWaitAny(EVENT_MASK(0)))
        {
            eventflags_t keys;
            keys = chEvtGetAndClearFlags(&elKbd);

            if (keys & KEY_PGM_UP && pgmval < 9999)
            {
                pgmval++;
            }
            else if (keys & KEY_PGM_DOWN && pgmval > 0)
            {
                pgmval--;
            }

            if (keys & KEY_O)
            {
                pgmval = 0;
            }

            if (keys & KEY_MAN_UP && absval < 9999)
            {
                absval++;
            }
            else if (keys & KEY_MAN_DOWN && absval > 0)
            {
                absval--;
            }

            chsnprintf(upper, 5, "%04d", absval);
            chsnprintf(lower, 5, "%04d", pgmval);

            displayHandler(upper, lower);
        }
    }

    chThdExit(MSG_OK);
}

void sahavtcb(void *p)
{
    (void) p;

    osalSysLockFromISR();
    chEvtBroadcastFlagsI(&keyboardEvent, 0);
    chVTResetI(&sahaVt);
    osalSysUnlockFromISR();
}

void initSaha(void)
{
    chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(1024), "saha", NORMALPRIO+1, sahaThread, NULL);
    chVTSet(&sahaVt, MS2ST(2000), sahavtcb, NULL);
}
