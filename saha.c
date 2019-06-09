#include "hal.h"
#include <stdio.h>
#include "keyscan.h"
#include "saha.h"
#include "display.h"
#include "helpers.h"

virtual_timer_t sahaVt;
static uint8_t blink = 0x00;

static void displayHandler(char *upper, char *lower, uint8_t mode)
{
    if (mode == MODE_STOP)
    {
        displays[0].digits[0] = blink ? ' ' : 'S';
        displays[0].digits[1] = blink ? ' ' : 'E';
        displays[0].digits[2] = blink ? ' ' : 'I';
        displays[0].digits[3] = blink ? ' ' : 'S';
    }
    else
    {
        displays[0].digits[0] = upper[0];
        displays[0].digits[1] = upper[1];
        displays[0].digits[2] = upper[2];
        displays[0].digits[3] = upper[3];
    }

    displays[1].digits[0] = ((mode == MODE_SEL && blink) ? ' ' : lower[0]) | 0x80;
    displays[1].digits[1] = (mode == MODE_PGM && blink) ? ' ' : lower[1];
    displays[1].digits[2] = (mode == MODE_PGM && blink) ? ' ' : lower[2];
    displays[1].digits[3] = (mode == MODE_PGM && blink) ? ' ' : lower[3];

    updateDisplay();
}

static THD_FUNCTION(sahaThread, arg)
{
    (void)arg;

    char upper[5];
    char lower[5];

    uint16_t pgm[SLOTS] = {0};

    /* replace with read from eeprom */
    pgm[0] = 21;
    pgm[1] = 40;
    pgm[2] = 70;
    pgm[3] = 100;
    pgm[4] = 144;

    uint8_t mode = MODE_SEL;
    uint8_t sel = 0;

    uint16_t pgmval = pgm[sel];
    uint16_t absval = 0;

    event_listener_t elKbd;

    chEvtRegister(&keyboardEvent, &elKbd, 0);

    while (!chThdShouldTerminateX())
    {
        if (chEvtWaitAnyTimeout(EVENT_MASK(0), MS2ST(250)))
        {
            eventflags_t keys;
            keys = chEvtGetAndClearFlags(&elKbd);

            if (keys & KEY_STOP)
            {
                mode = MODE_STOP;
            }

            if (keys & KEY_I)
            {
                if (mode == MODE_SEL)
                {
                    mode = MODE_RUN;
                }
                else
                {
                    mode = MODE_SEL;
                }
                pgmval = pgm[sel];
            }
            else if (keys & KEY_II)
            {
                if (mode == MODE_PGM)
                {
                    pgm[sel] = pgmval;
                    mode = MODE_SEL;
                }
                else
                {
                    mode = MODE_PGM;
                    pgmval = pgm[sel];
                }
            }
            else if (keys & KEY_O)
            {
                absval = 0;
            }

            if (mode == MODE_PGM && keys & KEY_PGM_UP && pgmval < 9999)
            {
                pgmval++;
            }
            else if (mode == MODE_PGM && keys & KEY_PGM_DOWN && pgmval > 0)
            {
                pgmval--;
            }

            if (mode == MODE_SEL && keys & KEY_PGM_UP)
            {
                if (sel < SLOTS-1)
                {
                    sel++;
                }
                else
                {
                    sel = 0;
                }
                pgmval = pgm[sel];
            }
            else if (mode == MODE_SEL && keys & KEY_PGM_DOWN)
            {
                if (sel > 0)
                {
                    sel--;
                }
                else
                {
                    sel = SLOTS-1;
                }
                pgmval = pgm[sel];
            }

            if (mode == MODE_RUN && keys & KEY_PGM_UP)
            {
                if (absval < (9999-pgmval))
                {
                    absval += pgmval;
                }
            }
            else if (mode == MODE_RUN && keys & KEY_PGM_DOWN)
            {
                if (absval >= pgmval)
                {
                    absval -= pgmval;
                }
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
            chsnprintf(lower, 5, "%d%3d", sel+1, pgmval);

            displayHandler(upper, lower, mode);
        }
    }

    chThdExit(MSG_OK);
}

void sahavtcb(void *p)
{
    (void) p;

    blink = ~blink;

    osalSysLockFromISR();
    chEvtBroadcastFlagsI(&keyboardEvent, 0);
    chVTResetI(&sahaVt);
    chVTSetI(&sahaVt, MS2ST(250), sahavtcb, NULL);
    osalSysUnlockFromISR();
}

static void stopButtonHandler(void *arg)
{
    (void) arg;

    osalSysLockFromISR();
    chEvtBroadcastFlagsI(&keyboardEvent, KEY_STOP);
    osalSysUnlockFromISR();
}


void initSaha(void)
{
    chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(1024), "saha", NORMALPRIO+1, sahaThread, NULL);
    chVTSet(&sahaVt, MS2ST(2000), sahavtcb, NULL);

    palEnableLineEvent(LINE_IN_1, PAL_EVENT_MODE_RISING_EDGE);
    palSetLineCallback(LINE_IN_1, stopButtonHandler, NULL);
}
