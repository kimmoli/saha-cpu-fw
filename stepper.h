#ifndef _STEPPER_H
#define _STEPPER_H

struct StepperDriver
{
    PWMDriver *pwmp;
    uint8_t channel;
    uint32_t directionLine;
    uint32_t pulsecount;
    uint32_t pulsestop;
    uint32_t dir;
    bool running;
};

typedef struct StepperDriver StepperDriver;

extern StepperDriver STEPPERD1;
extern StepperDriver STEPPERD2;

#define DIR_DOWN     PAL_HIGH
#define DIR_UP       PAL_LOW

extern void initStepper(void);

extern void step(StepperDriver *stepper, int32_t val);

#endif

