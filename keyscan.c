#include "hal.h"
#include <stdio.h>
#include "keyscan.h"
#include "display.h"
#include "helpers.h"

virtual_timer_t keyboardScanVt;
event_source_t keyboardScanEvent;
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

            //PRINT("%04x %04x %04x - %d\n\r", prevkeys, newkeys, keys, repecount);

            prevkeys = newkeys;
        }
    }

    return keys;
}

static THD_FUNCTION(keyboardScanThread, arg)
{
    (void)arg;

    event_listener_t elKbdScan;

    chEvtRegister(&keyboardScanEvent, &elKbdScan, 0);

    while (!chThdShouldTerminateX())
    {
        if (chEvtWaitAny(EVENT_MASK(0)))
        {
            chEvtGetAndClearFlags(&elKbdScan);

            uint16_t keys = scanKeyboard(&keyboards[0]);

            if (keys != 0)
            {
                chEvtBroadcastFlags(&keyboardEvent, keys);
            }
        }
    }

    chThdExit(MSG_OK);
}


void kbdvtcb(void *p)
{
    (void) p;

    osalSysLockFromISR();

    chEvtBroadcastFlagsI(&keyboardScanEvent, 0);

    chVTResetI(&keyboardScanVt);
    chVTSetI(&keyboardScanVt, MS2ST(100), kbdvtcb, NULL);
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

    chEvtObjectInit(&keyboardScanEvent);
    chEvtObjectInit(&keyboardEvent);
    chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(1024), "keyboard", NORMALPRIO+1, keyboardScanThread, NULL);

    chVTSet(&keyboardScanVt, MS2ST(2000), kbdvtcb, NULL);
}
