#define _IN_ADC_C

#include "settings.h"
#include "AD770X.h"

//AD770X TM7705(2.5, PIN_ADC_CS, PIN_ADC_MOSI, PIN_ADC_MISO, PIN_ADC_CLK, PIN_ADC_RST);
AD770X TM7705(2.5, 10, PIN_ADC_MOSI, PIN_ADC_MISO, PIN_ADC_CLK, 10);

bool adc_waitdataready_timeout(byte channel, char *label) {
	// if label == NULL, no serial printing is done 
    int i=0;
	int lim=10000;
    if (label) {
    	Serial.print(label);
    	Serial.print(": ");
		Serial.println("Waiting for dataReady()");
	}
    while (!TM7705.dataReady(channel)) {
    	i++;
    	if (i>=lim) break;
    };
    if (i<lim) {
    	if (label) {
			Serial.print("Data finally became ready at ");
			Serial.println(i);
		}
		return true;
	} else {
		if (label) {
			Serial.print("Data never ready. Reached max at ");
			Serial.println(i);
		}
		return false;
	}
}

void setup_adc() {
	TM7705.reset();
	/* TM7705.readClockRegister(AD770X::CHN_AIN1); */
	/* TM7705.readClockRegister(AD770X::CHN_AIN2); */
    /* TM7705.setNextOperation(AD770X::REG_CLOCK, AD770X::CHN_AIN1, 0); */
    /* SPI.transfer(0x05); */
    /* TM7705.setNextOperation(AD770X::REG_CLOCK, AD770X::CHN_AIN2, 0); */
    /* SPI.transfer(0x05); */
	/* TM7705.readClockRegister(AD770X::CHN_AIN1); */
	/* TM7705.readClockRegister(AD770X::CHN_AIN2); */
	/* adc_waitdataready_timeout(AD770X::CHN_AIN1, "Before init 1"); */
	TM7705.init(AD770X::CHN_AIN1, AD770X::CLK_DIV_1, AD770X::UNIPOLAR, AD770X::GAIN_1, AD770X::UPDATE_RATE_500);
	/* TM7705.readClockRegister(AD770X::CHN_AIN1); */
	/* adc_waitdataready_timeout(AD770X::CHN_AIN1, "After init 1"); */
	/* TM7705.init(AD770X::CHN_AIN2, AD770X::CLK_DIV_2, AD770X::UNIPOLAR, AD770X::GAIN_4, AD770X::UPDATE_RATE_50); */
	/* adc_waitdataready_timeout(AD770X::CHN_AIN1, "After init 2"); */
	/* TM7705.readClockRegister(AD770X::CHN_AIN1); */
	/* TM7705.readClockRegister(AD770X::CHN_AIN2); */
	/* adc_waitdataready_timeout(AD770X::CHN_AIN1, "After inits and clock read"); */
}

// Returns 1 on failure, 0 on success
// Stores unsigned int in *val on success
int loop_adc(unsigned int *val) {
	// put your main code here, to run repeatedly:
	if (!adc_waitdataready_timeout(AD770X::CHN_AIN1, NULL))
		return 1;
	
	/* Serial.print("#"); */
	/* Serial.print(i++); */
	/* Serial.print(": "); */
	
	// double method, returns voltage
	//read twice to improve reliability
	/* double v1 = 0.0; */
	/* Serial.print("CH1: "); */
	/* while (!TM7705.dataReady(AD770X::CHN_AIN1)); */
	/* v1 = TM7705.readADResult(AD770X::CHN_AIN1, 1.25); // ref offest = 125 */
	/* while (!TM7705.dataReady(AD770X::CHN_AIN1)); */
	/* v1 = TM7705.readADResult(AD770X::CHN_AIN1, 1.25); */
	/* Serial.print(v1, 5); */
	/* Serial.println(""); */
	if (val) *val=TM7705.readADResultRaw(AD770X::CHN_AIN1);
	return 0;
	
	//read twice to improve reliability
	/* double v2 = 0.0; */
	/* Serial.print("  CH2: "); */
	//while (!TM7705.dataReady(AD770X::CHN_AIN2)); // readADResult() already waits for dataReady()
	/* v2 = TM7705.readADResult(AD770X::CHN_AIN2, 1.25); */
	/* while (!TM7705.dataReady(AD770X::CHN_AIN2)); */
	/* v2 = TM7705.readADResult(AD770X::CHN_AIN2, 1.25); */
	/* Serial.print(v2, 5); */
	/* Serial.println(""); */
	/* delay(100); */
}
