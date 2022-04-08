//#define DO_GDB
#ifdef DO_GDB
	#include <GDBStub.h>
#endif
#include "settings.h"
#include "wifi.h"
#include "wifi_config.h"
#include "ota.h"
#include "printutils.h"
#include "netdata.h"
#include "adc.h"

unsigned long bauds[] = { 2400, 9600, 19200, 38400, 57600, 74880,
                          115200, 230400, 250000, 500000 };
#define BAUDSCNT (sizeof(bauds) / sizeof(*bauds))
//uint8_t baudi=5; // (5 => 74880 (bootloader baud), to see esp8266 bootloader msgs
                   // but apparently this isn't supported by my pc
uint8_t baudi=6; // 6 => 115200
unsigned long us_last_sample=micros();
unsigned long us_last_disconnect_notice=millis();
int netdata_pause=0;

uint8_t btn1state = BTN_MODE_NORMAL;
unsigned long btn1ctr = 0;
unsigned long btn1time = 0;
volatile bool int_btn1_trigged = false;
bool led1_state = false;

bool leads_disc_flag = false;

void baudnext() {
	if (baudi < BAUDSCNT-1) {
		baudi++;
		serial_reconnect();
	} else {
		spl("No higher bauds configured");
	}
}
void baudprior() {
	if (baudi > 0) {
		baudi--;
		serial_reconnect();
	} else {
		spl("No higher bauds configured");
	}
}
void serial_reconnect() {
	sp("Changing baud to "); // probably won't display since .end() will kick in
	spl(bauds[baudi]);
	// .flush() is currently needed (2021-01-07) for my esp8266 core, but
	// in the future maybe .end() flushes first.
	Serial.flush();
	Serial.end();
	Serial.begin(bauds[baudi]);
}

void setup () {
// initialize the serial communication:
	Serial.begin(bauds[baudi]);
	#ifdef DO_GDB
		gdbstub_init();
	#endif
	setup_wifi();
	setup_ota();
	setup_adc();
	setup_netdata();
	//pinMode(PIN_LO_PLUS, INPUT);  // Setup for leads off detection LO +
	pinMode(PIN_LO_MINUS, INPUT); // Setup for leads off detection LO -

	// Used for direct connection to AD8232. We're not doing direct.
	/* pinMode(PIN_SDN, OUTPUT);     // Driving LOW shuts down EKG AD8232 board */
	/* digitalWrite(PIN_SDN, HIGH);  // */

	pinMode(PIN_LED1, OUTPUT);
	digitalWrite(PIN_LED1, LOW);

	pinMode(PIN_BTN1, INPUT_PULLDOWN_16);
	/* attachInterrupt(digitalPinToInterrupt( PIN_BTN1 ), int_hand_btn1_change, CHANGE ); */
	int_btn1_trigged = BTN1_TRIGGED();

	sp(F("Free heap: "));
	spl(ESP.getFreeHeap());
}

void led1_toggle() {
	led1_state = !led1_state;
	digitalWrite(PIN_LED1, led1_state ? HIGH : LOW);
}
void led1_on() {
	led1_state = true;
	digitalWrite(PIN_LED1, HIGH);
}
void led1_off() {
	led1_state = false;
	digitalWrite(PIN_LED1, LOW);
}

unsigned int avg(int *vals, unsigned char cnt) {
	int32_t tot=0;
	for (unsigned char i=0; i<cnt; i++) tot += vals[i];
	tot /= cnt;
	return (unsigned int) tot;
}

void btn1_down() {
	spl("BUTTON1 Down");
	led1_on();
	btn1time = millis();
	netdata_send_btn1();
	/* led1_toggle(); */
}
void btn1_up() {
	sp("/BUTTON1 was down for ");
	sp((millis()-btn1time)/(float)1000);
	spl(" seconds");
	led1_off();
}

/* ICACHE_RAM_ATTR void int_hand_btn1_change() { */
/* 	int_btn1_trigged = !int_btn1_trigged; */
/* } */

void loop_button() {
	unsigned long cmillis = millis();
	static unsigned long last_loop_button_ms = cmillis;
	if (cmillis - last_loop_button_ms < 30) { // slow down testing
		return;
	} else {
		last_loop_button_ms = cmillis;
		int_btn1_trigged = BTN1_TRIGGED();
	}
	/* sp("BTN1: "); */
	/* spl(int_btn1_trigged); */
	// We're using HW debounce, but software debounce is handled
	// below for those who don't include the HW debounce components:
	if (btn1state == BTN_MODE_NORMAL) {
		if (int_btn1_trigged) {             // button pressed
			btn1state = BTN_MODE_DEBOUNCE;
			btn1ctr = cmillis;
		}
	} else if (btn1state == BTN_MODE_DEBOUNCE) {
		if (cmillis - btn1ctr > BTN_DEBOUNCE_MS) {
			if (int_btn1_trigged) {         // button still pressed
				btn1_down();
				btn1state = BTN_MODE_DOWN;
			} else {                         // it was noise or something
				btn1state = BTN_MODE_NORMAL; // don't consider it a press
			}
		}
	} else if (btn1state == BTN_MODE_DOWN) {
		// we only get here if the down-press was debounced
		if (int_btn1_trigged) {         // button still pressed
		} else {                        // no longer pressed, so debounce lift
			btn1state = BTN_MODE_UP_DEBOUNCE;
			btn1ctr = cmillis;
		}
	} else if (btn1state == BTN_MODE_UP_DEBOUNCE) {
		if (cmillis - btn1ctr > BTN_DEBOUNCE_MS) {
			if (!int_btn1_trigged) { // button stayed up, so button_up
				btn1_up();
				btn1state = BTN_MODE_NORMAL;
			} else {
				// after this time, the button is back down, so go
				// back to the down mode
				btn1state = BTN_MODE_DOWN;
			}
		}
	}
}

void loop_serial() {
	unsigned long cur_millis = millis();
	static unsigned long last_ser_ms = cur_millis;
	if (last_ser_ms - cur_millis > 100) {
		last_ser_ms = cur_millis;
		if (Serial.available() > 0) {
			int ichar;
			ichar = Serial.read();
			switch (ichar) {
				case '+': baudnext(); break;
				case '-': baudprior(); break;
				case 'p': netdata_pause = !netdata_pause; break;
				default:
					spl("Help: -/+ change baud. (p)ause data.");
					break;
			}
		}
	}
}

void loop() {
	//static int slow=0;
	//static int avv[AVGCNT];
	//static uint8_t avvi=0;
	uint32_t cmicros = micros();
	uint32_t cmillis = millis();

	/* This is useful for testing just the reading/ADC */
	/* if (cmicros-us_last_sample >= US_SAMPLES) { */
	/* 	us_last_sample = cmicros; */
	/* 	loop_adc(); */
	/* } */
	/* return; */

	if (cmicros-us_last_sample >= US_SAMPLES) {
		us_last_sample = cmicros;
		/* if ((digitalRead(PIN_LO_PLUS) == 1) || */
		/* 		(digitalRead(PIN_LO_MINUS) == 1)) { */
			/* Serial.println('!');  // lots of output if no sensor leads */
		
		/* Only checking one electrode right now. No free pin for the
		   other */
		if (digitalRead(PIN_LO_MINUS) == 1) {
			if (cmillis-us_last_disconnect_notice >= MS_DISC_NOTE) {
				us_last_disconnect_notice = cmillis;
				leads_disc_flag = true;
				spl(F("Electrode LO- is disconnected."));
			}
		} else {
			unsigned int v;
			if (leads_disc_flag) {
				leads_disc_flag = false;
				spl(F("Electrode LO- REconnected"));
			}
			/* v=analogRead(PIN_OUTPUT); */
			if (!loop_adc(&v)) { // 0 == success
				#ifdef PLOT_TO_SERIAL
					//avv[avvi] = v;
					//if (++avvi >= AVGCNT) avvi=0;
					//slow = avg(avv, AVGCNT);
					Serial.println(v);
					/* Serial.print('\t'); */
					/* Serial.println(slow); */
				#endif
				#ifdef SEND_TO_NET
					if (wifi_connflags & WIFI_FLAG_CONNECTED) {
						netdata_add(v);
					}
				#endif
			}
		}
	}
	// \/ came from some webpage. We have enough other stuff going on though
	//delay (1); //Wait for a bit to keep serial data from saturating
	//loop_ota();
	loop_wifi();
	//wifi_connflags = loop_check_wifi();
	/* if (wifi_connflags & WIFI_FLAG_IGNORE) {} */
	/* else { */
	/* 	if ((wifi_connflags & WIFI_FLAG_CONNECTED)) ws_net_connect(); */
	/* 	else                                   ws_net_disconnected(); */
	/* } */
		//#warning "Netdata paused in ecg.ino"
	if (!netdata_pause) loop_netdata(); 
	loop_button();
	/* loop_serial(); */
}
