#define _IN_ADC_C

#include "settings.h"
#include "AD770X.h"

//AD770X TM7705(2.5, PIN_ADC_CS, PIN_ADC_MOSI, PIN_ADC_MISO, PIN_ADC_CLK, PIN_ADC_RST);
AD770X TM7705(2.5, PIN_ADC_MOSI, PIN_ADC_MISO, PIN_ADC_CLK);

void setup_adc() {
	TM7705.readClockRegister(AD770X::CHN_AIN1);
	TM7705.readClockRegister(AD770X::CHN_AIN2);
	TM7705.reset();
	return;
	TM7705.init(AD770X::CHN_AIN1, AD770X::CLK_DIV_1, AD770X::BIPOLAR, AD770X::GAIN_8, AD770X::UPDATE_RATE_200);
	// For Input 2
	TM7705.init(AD770X::CHN_AIN2, AD770X::CLK_DIV_1, AD770X::BIPOLAR, AD770X::GAIN_4, AD770X::UPDATE_RATE_200);
}

void loop_adc() {
	return;
	// put your main code here, to run repeatedly:
	static int i = 0;
	double v1 = 0.0;
	double v2 = 0.0;
	if (!TM7705.dataReady(AD770X::CHN_AIN1)) return;
	
	Serial.print("#");
	Serial.print(i++);
	Serial.print(": ");
	
	//read twice to improve reliability
	Serial.print("CH1: ");
	/* while (!TM7705.dataReady(AD770X::CHN_AIN1)); */
	v1 = TM7705.readADResult(AD770X::CHN_AIN1, 1.25); // ref offest = 125
	/* while (!TM7705.dataReady(AD770X::CHN_AIN1)); */
	/* v1 = TM7705.readADResult(AD770X::CHN_AIN1, 1.25); */
	Serial.print(v1, 5);
	/* Serial.println(""); */
	
	//read twice to improve reliability
	Serial.print("  CH2: ");
	//while (!TM7705.dataReady(AD770X::CHN_AIN2)); // readADResult() already waits for dataReady()
	v2 = TM7705.readADResult(AD770X::CHN_AIN2, 1.25);
	/* while (!TM7705.dataReady(AD770X::CHN_AIN2)); */
	/* v2 = TM7705.readADResult(AD770X::CHN_AIN2, 1.25); */
	Serial.print(v2, 5);
	Serial.println("");
	/* delay(1000); */
}
