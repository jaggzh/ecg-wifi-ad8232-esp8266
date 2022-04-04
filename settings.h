/* If we output ECG data to serial.
 * At the time of this writing this is the raw data (however fast we can
 * grab it), as well as a smoothed avg */
/* #define PLOT_TO_SERIAL */
#define SEND_TO_NET

// Wifi drops with these
/* #define US_SAMPLES  (1000000/100) // min uS between samples */
//#define US_SAMPLES  (1000000/1000) // min uS between samples
//#define US_SAMPLES  (1000000/2000) // min uS between samples
//#define US_SAMPLES  (1000000/500) // min uS between samples

// Wifi works with these
//#define US_SAMPLES  (1000000/100) // min uS between samples
#define US_SAMPLES  (1000000/250) // min uS between samples

/* For averaging out the raw data */
#define AVGCNT 6 // probably unused now.

/* #define PIN_LO_PLUS  D5  // d5 gpio14 (sck) */
/* #define PIN_LO_MINUS D4  // d4 gpio2 */
/* #define PIN_SDN      D7  // d7 gpio13 (mosi) */
/* #define PIN_OUTPUT   A0  // A0 (adc0) */
/* #define PIN_BTN1     D0  // D0 gpio16 */

// For D1 Mini I have:
//  LO+ -> D5, LO- -> D4.  Its "Output" signal goes to my D1Mini's A0 pin.
#define PIN_OUTPUT   A0  // A0 (adc0)
#define PIN_LO_MINUS D2  // d4 gpio2
/* lo+ is disabled for now, because I don't have a pin free for it */
/* #define PIN_LO_PLUS  D5  // d5 gpio14 (sck) */
#define PIN_SDN      D7  // d7 gpio13 (mosi)

#define PIN_LED1     D1  // D1 gpio5
#define PIN_BTN1     D0  // D6 gpio16
/* Use this to set the HIGH/LOW logic: */
#define BTN1_TRIGGED() (digitalRead(PIN_BTN1) ? true : false)

// Internal use
#define BTN_MODE_NORMAL      0
#define BTN_MODE_DEBOUNCE    1
#define BTN_MODE_DOWN        2
#define BTN_MODE_UP          3
#define BTN_MODE_UP_DEBOUNCE 4
#define BTN_DEBOUNCE_MS    50
