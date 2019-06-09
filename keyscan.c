#include "hal.h"
#include <stdio.h>
#include "keyscan.h"
#include "display.h"
#include "helpers.h"
#include "sahakeys.h"

virtual_timer_t keyboardVt;
event_source_t keyboardEvent;

KeyboardDriver keyboards[8];

static uint16_t scanKeyboard(KeyboardDriver *kbd)
{
    static volatile uint16_t prevkeys;
    static volatile uint16_t repecount;
    uint16_t keys = 0;
    uint16_t newkeys = 0;

    uint8_t rxBuf[5];
    msg_t ret;

    if (kbd->active)
    {
        i2cAcquireBus(&I2CD1);
        i2cMasterTransmit(&I2CD1, kbd->i2cAddress, (uint8_t[]){ 0x41 }, 1, NULL, 0);
        ret = i2cMasterReceive(&I2CD1, kbd->i2cAddress, rxBuf, 5);
        i2cReleaseBus(&I2CD1);

        if (ret == MSG_OK)
        {
            newkeys = (rxBuf[0] & 0x1f) | ((rxBuf[2] & 0x1f) << 5) | ((rxBuf[4] & 0x1f) << 10);

            if (newkeys != 0 && newkeys == prevkeys)
            {
                if (repecount > 10)
                {
                    keys = newkeys;
                }
                else
                {
                    repecount++;
                }
            }
            else
            {
                keys = (newkeys ^ prevkeys) & newkeys;
                repecount = 0;
            }

            PRINT("%04x %04x %04x - %d\n\r", prevkeys, newkeys, keys, repecount);

            prevkeys = newkeys;
        }
    }

    return keys;
}

static THD_FUNCTION(keyboardThread, arg)
{
    (void)arg;
    char buf[10] = {0};
    uint16_t pgmval = 0;

    event_listener_t elKbd;

    chEvtRegister(&keyboardEvent, &elKbd, 0);

    while (!chThdShouldTerminateX())
    {
        if (chEvtWaitAny(EVENT_MASK(0)))
        {
            eventflags_t flags;
            flags = chEvtGetAndClearFlags(&elKbd);

            uint32_t keys = scanKeyboard(&keyboards[0]);

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

            chsnprintf(buf, 6, "%04d", pgmval);

            displays[1].digits[0] = buf[0];
            displays[1].digits[1] = buf[1];
            displays[1].digits[2] = buf[2];
            displays[1].digits[3] = buf[3];

            updateDisplay();
        }
    }

    chThdExit(MSG_OK);
}


void kbdvtcb(void *p)
{
    (void) p;

    osalSysLockFromISR();

    chEvtBroadcastFlagsI(&keyboardEvent, 0);

    chVTResetI(&keyboardVt);
    chVTSetI(&keyboardVt, MS2ST(100), kbdvtcb, NULL);
    osalSysUnlockFromISR();
}

void initKeyboard(void)
{
    /* Each display has a keyboard. No other keyboards exists. */

    for (int i = 0; i < 8 ; i++)
    {
        if (displays[i].active)
        {
            keyboards[i].active = true;
            keyboards[i].i2cAddress = displays[i].i2cAddress;
        }
        else
        {
            keyboards[i].active = false;
        }
    }

    chEvtObjectInit(&keyboardEvent);
    chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(1024), "keyboard", NORMALPRIO+1, keyboardThread, NULL);

    chVTSet(&keyboardVt, MS2ST(1000), kbdvtcb, NULL);
}
