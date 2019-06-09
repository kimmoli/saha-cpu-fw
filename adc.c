#include "hal.h"
#include "adc.h"
#include "dmm.h"

virtual_timer_t adcVt;

float batv = 0.0;

static void adccb1(ADCDriver *adcp, adcsample_t *buffer, size_t n);
static void adcvtcb(void *p);

const ADCConversionGroup adcgrpcfg1 =
{
    /* Circular */ FALSE,
    /* numbr of channeld */ ADC_GRP1_NUM_CHANNELS,
    /* conversion end callback*/ adccb1,
    /* conversion error callback */ NULL,
    /* ADC CR1 */ 0,
    /* ADC CR2 */ ADC_CR2_SWSTART,
    /* ADC SMPR1 */ 0,
    /* ADC SMPR2 */ ADC_SMPR2_SMP_AN7(ADC_SAMPLE_144),
    /* ADC SQR1 */ ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
    /* ADC SQR2 */ 0,
    /* ADC SQR3 */ ADC_SQR3_SQ1_N(ADC_CHANNEL_IN7)
};

static adcsample_t adcSamples1[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

/*
 * ADC end conversion callback, ADC1
 */
void adccb1(ADCDriver *adcp, adcsample_t *buffer, size_t n)
{
    (void) buffer;
    (void) n;
    int i;

    /* Note, only in the ADC_COMPLETE state because the ADC driver fires an intermediate callback when the buffer is half full.*/
    if (adcp->state == ADC_COMPLETE)
    {
        /* Calculates the average values from the ADC samples.*/
        adcsample_t adcAvgSupplyVoltage = 0;

        for (i = 0; i < ADC_GRP1_BUF_DEPTH; i++)
        {
            adcAvgSupplyVoltage += adcSamples1[ i * ADC_GRP1_NUM_CHANNELS];
        }

        adcAvgSupplyVoltage /= ADC_GRP1_BUF_DEPTH;

        batv = ADC_MEAS12V_SCALE * (float)adcAvgSupplyVoltage;

        osalSysLockFromISR();

        chEvtBroadcastFlagsI(&dmmUpdate, 0);

        osalSysUnlockFromISR();
    }
}

void adcvtcb(void *p)
{
    (void) p;

    osalSysLockFromISR();

    chVTResetI(&adcVt);
    chVTSetI(&adcVt, MS2ST(100), adcvtcb, NULL);
    adcStartConversionI(&ADCD1, &adcgrpcfg1, adcSamples1, ADC_GRP1_BUF_DEPTH);

    osalSysUnlockFromISR();
}

void initAdc(void)
{
    batv = 0.0;

    adcStart(&ADCD1, NULL);
//    adcSTM32EnableTSVREFE();

    chVTSet(&adcVt, MS2ST(2000), adcvtcb, NULL);
}
