/* If we output ECG data to serial.
 * At the time of this writing this is the raw data (however fast we can
 * grab it), as well as a smoothed avg */
//#define PLOT_TO_SERIAL
#define SEND_TO_WEBSOCKET
#define US_SAMPLES  (1000000/100) // min uS between samples

/* For averaging out the raw data */
#define AVGCNT 6

