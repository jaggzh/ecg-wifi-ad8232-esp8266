#include <ctype.h>

#define _IN_NETDATA_C
#include "netdata.h"

#define NETDATA_SETTINGS_MAIN // declares char[] stmag and enmag
#include "netdata-settings.h"
#include "wifi_config.h"
#include "serialize.h"

WiFiClient server;

unsigned long us_last_nettest=micros();

/* struct ecg_packet { */
/* 	uint32_t us; */
/* 	uint16_t val; */
/* }; */
/* // Cluster packets to maximum of roughly one TCP packet */
/* // (not yet sure if esp8266 can handle this) */
/* #define PACKETCNT (1500/sizeof(struct ecg_packet)) */
/* struct ecg_packet packets[PACKETCNT]; */
/* int nextpacketi=0; */


// Full package to send out to net
// Includes: start and end MAGIC
//           package type
//           data
uint8_t ecg_fullpackage[FULLPAK_SIZE+1]; // +1 for safety. it's unused.
// \/ Points to where the data actually starts
uint8_t *ecg_netdata = ecg_fullpackage + MAGIC_SIZE + PAKTYPE_SIZE;
int nextpacketi=0;

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

void netdata_send() {
	//Serial.println("netdata_send()");
	/* Serial.print("Free heap: "); */
	/* Serial.println(ESP.getFreeHeap()); */
	if (!server.connected()) {
		//Serial.println("Not connected. Losing data");
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
	unsigned long us_cur=micros();
	if (us_cur - us_last_nettest > US_NETDATA_TEST) {
		us_last_nettest -= US_NETDATA_TEST;
		if (!server.connected()) {
			Serial.println("TCP Connecting...");
			if (server.connect(DATA_HOST, DATA_PORT)) {
				Serial.println(" Connected!");
				server.write("login 123456\n", 13);
			}
		} else {
			// netdata_send_check();
		}
	}
}
