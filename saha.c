#include "hal.h"
#include <stdio.h>
#include "keyscan.h"
#include "saha.h"
#include "display.h"
#include "helpers.h"
#include "stepper.h"

virtual_timer_t sahaVt;
static uint8_t blink = 0x00;

static void displayHandler(char *upper, char *lower, uint16_t mode)
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
        displays[0].digits[0] = upper[0] | (blink && mode == MODE_RUN ? 0x80 : 0x00);
        displays[0].digits[1] = upper[1] | (blink && mode == MODE_RUN ? 0x80 : 0x00);
        displays[0].digits[2] = upper[2] | (blink && mode == MODE_RUN ? 0x80 : 0x00);
        displays[0].digits[3] = upper[3] | (blink && mode == MODE_RUN ? 0x80 : 0x00);
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

    uint16_t mode = MODE_SEL;
    uint8_t sel = 0;

    int16_t pgmval = pgm[sel];
    int16_t absval = 0;
    int16_t setval = 0;
    int16_t prevval = 0;

    event_listener_t elKbd;

    chEvtRegister(&keyboardEvent, &elKbd, 0);

    while (!chThdShouldTerminateX())
    {
        if (chEvtWaitAnyTimeout(EVENT_MASK(0), MS2ST(250)))
        {
            eventflags_t keys;
            keys = chEvtGetAndClearFlags(&elKbd);

            if (STEPPERD1.running)
            {
                absval = prevval + ((STEPPERD1.pulsecount / PULSESPERMM) * ((STEPPERD1.dir == DIR_UP) ? 1 : -1));
            }

            if (keys & KEY_STOP)
            {
                step(&STEPPERD1, 0);
                mode = MODE_STOP;
                prevval = absval;
                setval = absval;
            }

            if (keys & KEY_DONE && mode == MODE_RUN)
            {
                mode = MODE_READY;
                prevval = setval;
                absval = setval;
            }

            if (mode != MODE_RUN)
            {
                if (keys & KEY_I)
                {
                    if (mode == MODE_SEL)
                    {
                        mode = MODE_READY;
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
                    setval = 0;
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

                if (mode == MODE_READY && keys & KEY_PGM_UP)
                {
                    if (setval < (9999-pgmval))
                    {
                        setval += pgmval;
                    }
                }
                else if (mode == MODE_READY && keys & KEY_PGM_DOWN)
                {
                    if (setval >= pgmval)
                    {
                        setval -= pgmval;
                    }
                }

                if (keys & KEY_MAN_UP && setval < 9999 && mode == MODE_READY)
                {
                    setval++;
                }
                else if (keys & KEY_MAN_DOWN && setval > 0 && mode == MODE_READY)
                {
                    setval--;
                }
            }

            chsnprintf(upper, 5, "%04d", absval);
            chsnprintf(lower, 5, "%d%3d", sel+1, pgmval);

            displayHandler(upper, lower, mode);

            if (prevval != setval && mode == MODE_READY)
            {
                step(&STEPPERD1, (setval - prevval) * PULSESPERMM);
                mode = MODE_RUN;
            }
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
