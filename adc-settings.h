#ifndef _ADC_SETTINGS_H
#define _ADC_SETTINGS_H

/* Uncomment either or both of the following DISABLE options
 * if you're not using these features/pins/wires.
 *
 * CS is required with multiple SPI devices attached to your controller, to
 * choose which one is being communicated with.  Thus, if you are only
 * communicating with the AD770x board, it can just be grounded (according to
 * the ad7705 datasheet).  (In SPI protocol, setting an SPI device's CS LOW is
 * what activates that one for communication.  All others are HIGH.)
 *
 * RST is needed for some reset; I'm not sure what this does and haven't looked
 * into it yet; however, the ad7705 datasheet says the minimum use-case (3-pin)
 * doesn't require it, in which case RST should be set high.  In my project I
 * therefore wire CS to GND and RST to VIn.
 */
#define AD770X_DISABLE_CS
#define AD770X_DISABLE_RST

/* 3-wire mode follows.  Undefine the CS or RST if you want to use those */
/* #define PIN_ADC_CS  // unused in this project (goes to gnd) */
#define PIN_ADC_MOSI D7
#define PIN_ADC_MISO D6
#define PIN_ADC_CLK  D5
/* #define PIN_ADC_RST // unused in this project (goes to Vcc) */


#endif
