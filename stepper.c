#include "hal.h"
#include "stepper.h"
#include "helpers.h"
#include <stdlib.h>
#include "saha.h"
#include "keyscan.h"

event_source_t stepperEvent;

StepperDriver STEPPERD1;
StepperDriver STEPPERD2;

static void stepper1Cb(PWMDriver *pwmp)
{
    (void) pwmp;
    STEPPERD1.pulsecount++;

    if (STEPPERD1.pulsecount == STEPPERD1.pulsestop)
    {
        osalSysLockFromISR();
        pwmDisableChannelNotificationI(STEPPERD1.pwmp, STEPPERD1.channel);
        pwmDisableChannelI(STEPPERD1.pwmp, STEPPERD1.channel);
        STEPPERD1.running = false;
        chEvtBroadcastFlagsI(&keyboardEvent, KEY_DONE);
        osalSysUnlockFromISR();
    }
}

static void stepper2Cb(PWMDriver *pwmp)
{
    (void) pwmp;
    STEPPERD2.pulsecount++;

    if (STEPPERD2.pulsecount == STEPPERD2.pulsestop)
    {
        osalSysLockFromISR();
        pwmDisableChannelNotificationI(STEPPERD2.pwmp, STEPPERD2.channel);
        pwmDisableChannelI(STEPPERD2.pwmp, STEPPERD2.channel);
        STEPPERD2.running = false;
        chEvtBroadcastFlagsI(&keyboardEvent, KEY_DONE);
        osalSysUnlockFromISR();
    }
}

const PWMConfig stepperConfig =
{
    1000000,                                    /* 1 MHz  */
    500,                                        /* / 500 = 2 kHz */
    NULL,
    {
        {PWM_OUTPUT_ACTIVE_HIGH, stepper1Cb},
        {PWM_OUTPUT_ACTIVE_HIGH, stepper2Cb},
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL},
    },
    0,
    0
};

static THD_FUNCTION(stepperThread, arg)
{
    (void) arg;

    event_listener_t elStepper;
    chEvtRegister(&stepperEvent, &elStepper, 0);

    while (!chThdShouldTerminateX())
    {
        chEvtWaitOne(EVENT_MASK(0));
    }

    chThdExit(MSG_OK);
}

void step(StepperDriver *stepper, int32_t val)
{
    if (!pwmIsChannelEnabledI(stepper->pwmp, stepper->channel))
    {
        if (val != 0 && !stepper->running)
        {
            if ( val > 0 )
            {
                stepper->dir = DIR_UP;
            }
            else
            {
                stepper->dir = DIR_DOWN;
            }

            palWriteLine(stepper->directionLine, stepper->dir);
            stepper->pulsecount = 0;
            stepper->pulsestop = abs(val);
            stepper->running = true;
            pwmEnableChannel(stepper->pwmp, stepper->channel, PWM_PERCENTAGE_TO_WIDTH(stepper->pwmp, 5000));
            pwmEnableChannelNotification(stepper->pwmp, stepper->channel);
        }
    }

    if (val == 0)
    {
        pwmDisableChannelNotificationI(stepper->pwmp, stepper->channel);
        pwmDisableChannelI(stepper->pwmp, stepper->channel);
        stepper->running = false;
    }
}

void initStepper(void)
{
    STEPPERD1.pwmp = &PWMD9;
    STEPPERD1.channel = 0;
    STEPPERD1.directionLine = LINE_DIR_1;
    STEPPERD1.running = false;

    STEPPERD2.pwmp = &PWMD9;
    STEPPERD2.channel = 1;
    STEPPERD2.directionLine = LINE_DIR_2;
    STEPPERD2.running = false;

    pwmStart(&PWMD9, &stepperConfig);

    palClearLine(LINE_OE_N);

    chEvtObjectInit(&stepperEvent);
    chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(2048), "stepper", NORMALPRIO+1, stepperThread, NULL);
}
