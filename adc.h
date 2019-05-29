#ifndef _ADC_H
#define _ADC_H

/* Total number of channels to be sampled by a single ADC operation.*/
#define ADC_GRP1_NUM_CHANNELS   1

/* Depth of the conversion buffer, channels are sampled n times each.*/
#define ADC_GRP1_BUF_DEPTH      16

extern event_source_t adcConvStart;

extern float batv;

void initAdc(void);

#endif //K_ADC_H
