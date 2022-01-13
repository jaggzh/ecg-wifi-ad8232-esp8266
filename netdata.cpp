#include <ctype.h>

#include "netdata.h"
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


// Matches client .ino
#define PAK_SIZE (4+2)
#define MAX_PACKETS  (1000/PAK_SIZE) // fit within ESP's packet (524?)
uint8_t ecg_netdata[(4+2)*MAX_PACKETS];
#define PAKS_LEN sizeof(ecg_netdata)
int nextpacketi=0;

uint8_t stmag[]=MAGIC_ST;
uint8_t enmag[]=MAGIC_EN;

void setup_netdata() {

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

	packi32(ecg_netdata + (nextpacketi*PAK_SIZE), us);
	packi16(ecg_netdata + (nextpacketi*PAK_SIZE) + 4, v);
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
		server.write(stmag, sizeof(stmag));
		server.write(ecg_netdata, sizeof(ecg_netdata));
		server.write(enmag, sizeof(enmag));
		/* Serial.println(""); */
		Serial.print("Data size: ");
		Serial.println(sizeof(ecg_netdata) + sizeof(stmag) + sizeof(enmag));
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
