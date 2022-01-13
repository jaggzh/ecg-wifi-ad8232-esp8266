#include <GDBStub.h>
#include "settings.h"
#include "wifi.h"
#include "wifi_config.h"
#include "ota.h"
#include "printutils.h"
#include "netdata.h"

unsigned long bauds[] = { 2400, 9600, 19200, 38400, 57600, 115200, 230400, 250000, 500000 };
#define BAUDSCNT (sizeof(bauds) / sizeof(*bauds))
uint8_t baudi=2; // start at this baud
uint32_t us_last_sample=micros();
int netdata_pause=0;

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
	gdbstub_init();
	setup_wifi();
	setup_ota();
	setup_netdata();
	pinMode(14, INPUT);  // Setup for leads off detection LO +
	pinMode(12, INPUT);  // Setup for leads off detection LO -
}

unsigned int avg(int *vals, unsigned char cnt) {
	int32_t tot=0;
	for (unsigned char i=0; i<cnt; i++) tot += vals[i];
	tot /= cnt;
	return (unsigned int) tot;
}

void loop () {
	//static int slow=0;
	//static int avv[AVGCNT];
	//static uint8_t avvi=0;
	uint32_t cmicros = micros();

	if (cmicros-us_last_sample >= US_SAMPLES) {
		// Use to keep as fast as possible, but
		// with a minimum time between sample readings:
		// us_last_sample -= US_SAMPLES;

		// Use to give an actual delay, regardless of
		// other overhead:
		us_last_sample = cmicros;

		if ((digitalRead(10) == 1) || (digitalRead(11) == 1)) {
			Serial.println('!');
		} else {
			int v;
			v=analogRead(A0);
			#ifdef PLOT_TO_SERIAL
				//avv[avvi] = v;
				//if (++avvi >= AVGCNT) avvi=0;
				//slow = avg(avv, AVGCNT);
				Serial.print(v);
				Serial.print('\t');
				Serial.println(slow);
			#endif
			#ifdef SEND_TO_NET
				netdata_add(v);
			#endif
			// send the value of analog input 0:
		}
	}
	// \/ came from some webpage. We have enough other stuff going on though
	//delay (1); //Wait for a bit to keep serial data from saturating
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
	loop_ota();
	uint16_t connflags;
	connflags = loop_check_wifi();
	/* if (connflags & WIFI_FLAG_IGNORE) {} */
	/* else { */
	/* 	if ((connflags & WIFI_FLAG_CONNECTED)) ws_net_connect(); */
	/* 	else                                   ws_net_disconnected(); */
	/* } */
	if (connflags & WIFI_FLAG_CONNECTED) {
		if (!netdata_pause) loop_netdata();
	}
}
