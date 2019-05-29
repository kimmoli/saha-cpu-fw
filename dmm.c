#include "hal.h"
#include <stdio.h>
#include "i2c.h"
#include "adc.h"
#include <math.h>
#include "helpers.h"
#include "display.h"

event_source_t dmmUpdate;

static THD_FUNCTION(dmmThread, arg)
{
    (void)arg;

    event_listener_t elDmm;
    eventflags_t flags;
    char buf[10] = {0};

    chEvtRegister(&dmmUpdate, &elDmm, 0);

    while (!chThdShouldTerminateX())
    {
        chEvtWaitOne(EVENT_MASK(0));

        flags = chEvtGetAndClearFlags(&elDmm);

        chsnprintf(buf, 6, "%05.2f", batv);

        displays[0].digits[0] = buf[0];
        displays[0].digits[1] = buf[1] | 0x80;
        displays[0].digits[2] = buf[3];
        displays[0].digits[3] = buf[4];

        displayUpdate();
    }

    chThdExit(MSG_OK);
}

void initDmm(void)
{
    chEvtObjectInit(&dmmUpdate);

    chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(1024), "dmm", NORMALPRIO+1, dmmThread, NULL);
}

