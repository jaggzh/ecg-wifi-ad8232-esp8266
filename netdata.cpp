#include <ctype.h>

#define _IN_NETDATA_C
#include "netdata.h"

#define NETDATA_SETTINGS_MAIN // declares char[] stmag and enmag
#include "netdata-settings.h"
#include "wifi_config.h"
#include "wifi.h"
#include "serialize.h"
#include "printutils.h"

WiFiClient server;

unsigned long us_last_nettest=micros();

// Full package to send out to net
// Includes: start and end MAGIC
//           package type
//           data
uint8_t ecg_fullpackage[FULLPAK_SIZE+1]; // +1 for safety. it's unused.
// \/ Points to where the data actually starts
uint8_t *ecg_netdata = ecg_fullpackage + MAGIC_SIZE + PAKTYPE_SIZE;
int nextpacketi=0;

// Full button package
struct ecg_btn_package {
	uint8_t magic_st[MAGIC_SIZE] = MAGIC_ST;
	PAKTYPE_TYPE paktype[PAKTYPE_SIZE] = { PAK_T_BTN1 };
	uint8_t magic_en[MAGIC_SIZE] = MAGIC_EN;
} ecg_btn_package;

void setup_netdata() {
	static_assert(sizeof(stmag) == sizeof(enmag), "Let's keep MAGIC_ST and MAGIC_EN the same length");
	static_assert(PAKTYPE_SIZE == 1, "PAKTYPE_SIZE should be 1, or you'll need to modify the data sending to encode it for the network");
}

void netdata_add(uint16_t v) { // call to add value to send
	/* packets[nextpacketi].us = micros(); */
	/* packets[nextpacketi].val = v; */
	// void packi16(unsigned char *buf, unsigned int i)
	// void packi32(unsigned char *buf, unsigned long int i)
	uint32_t us;
	uint16_t testval;
	/* #warning "netdata_add() is disabled" */
	/* return; */
	//us = micros();
	us = millis();
	/* us = *((uint32_t *)"abcd"); */
	/* v = *((uint16_t *)"yz"); */
	/* us = 123456; */
	/* v = 45678; */

	packi32(ecg_netdata + (nextpacketi*SAMPLE_SIZE), us);
	packi16(ecg_netdata + (nextpacketi*SAMPLE_SIZE) + 4, v);
	/* Serial.print(" u:"); */
	/* Serial.print(packets[nextpacketi].us); */
	/* Serial.print(" v:"); */
	/* Serial.print(packets[nextpacketi].val); */
	/* Serial.print(". "); */
	nextpacketi++;
	if (nextpacketi >= MAX_PACKETS) {
		netdata_send();
		nextpacketi=0;
	}
}

void netdata_send_btn1() {
	if (!server.connected()) {
		Serial.println(F("TCP not connected. Button not sent"));
	} else {
		server.write((uint8_t *)(&ecg_btn_package), BTNPAK_SIZE);
		Serial.println(F("BTN1 sent"));
	}
}

void netdata_send() {
	if (!server.connected()) {
		// don't bother sending out msg if wifi's down, since it
		// should already be complaining through serial:
		Serial.println(F("TCP not connected. Losing data"));
	} else {
		/* Serial.print("[>"); */
		/* Serial.print(PACKETCNT); */
		/* Serial.print("="); */
		/* Serial.print(sizeof(packets)); */
		/* Serial.print("b "); */
		memcpy(ecg_fullpackage + PAKFULL_MAGIC_OFFSET, stmag, MAGIC_SIZE);
		// \/ we static_assert()ed that this is one byte:
		ecg_fullpackage[PAKFULL_TYPE_OFFSET] = PAK_T_DATA;

		//memcpy(ecg_fullpackage + PAKFULL_DATA_OFFSET, stmag, sizeof(stmag));
		// ^ data should already be in place from ecg_netdata pointing there
		// \/ here we have to add magic to the end:
		memcpy(ecg_fullpackage + PAKFULL_DATA_OFFSET + (nextpacketi*SAMPLE_SIZE),
				enmag, MAGIC_SIZE);
		
		/* No longer do as separate writes:
			server.write(stmag, sizeof(stmag));
			server.write(ecg_netdata, sizeof(ecg_netdata));
			server.write(enmag, sizeof(enmag)); */
		server.write(ecg_fullpackage,
				PAKFULL_DATA_OFFSET + (nextpacketi*SAMPLE_SIZE) + MAGIC_SIZE);
		/* Serial.println(""); */
		Serial.print("Data size: ");
		Serial.println(PAKFULL_DATA_OFFSET + (nextpacketi*SAMPLE_SIZE) + MAGIC_SIZE);
		//Serial.println(sizeof(ecg_netdata) + sizeof(stmag) + sizeof(enmag));
		//Serial.println("");
	}
}

void loop_netdata() {
	unsigned long us_cur = micros();
	if (wifi_connflags & WIFI_FLAG_CONNECTED) { // only bother if wifi connected
		if (us_cur - us_last_nettest > US_NETDATA_TEST) {
			us_last_nettest = us_cur;
			if (!server.connected()) {
				Serial.println("TCP Connecting...");
				if (server.connect(DATA_HOST, DATA_PORT)) {
					Serial.println(" TCP Connected! Logging in...");
					server.write("login 123456\n", 13);
					Serial.println(" TCP Hopefully login worked, but we're moving on now...");
				}
				Serial.println("/TCP Connecting");
			} else {
				// netdata_send_check(); // now handled by netdata_add()
			}
		}
	}
}
