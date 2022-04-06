#ifndef _ADC_H
#define _ADC_H
#include "adc-settings.h"
#include "AD770X.h"

#ifndef _IN_ADC_C
// extern ? AD770X TM7705;
#endif

void setup_adc();
void loop_adc();

#endif
