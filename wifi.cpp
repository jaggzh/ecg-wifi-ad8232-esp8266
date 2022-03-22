#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define WIFI_CONFIG_GET_IPS
#define __WIFI_CPP
#include "wifi_config.h"
#define _IN_WIFI_CPP
#include "wifi.h"
#include "printutils.h"

uint16_t wifi_connflags = 0;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
WiFiEventHandler wifiGotIPHandler;

void loop_wifi(void) {
	/* long rssi = WiFi.RSSI(); */
	/* unsigned long cur_millis = millis(); */
	/* static unsigned long last_wifi_strength = cur_millis; */
	/* if (cur_millis - last_wifi_strength > 500) { */
	/* 	last_wifi_strength = cur_millis; */
	/* 	Serial.print("WiFi strength: "); */
	/* 	Serial.println(rssi); */
	/* } */
}

void setup_wifi(void) {
	WiFi.mode(WIFI_STA);
	WiFi.config(ip, gw, nm);
	WiFi.setOutputPower(20.5); // 0 - 20.5 (multiples of .25)
	sp(F("Connecting to wife (WiFi.begin())..."));
	wifiConnectHandler = WiFi.onStationModeConnected(onWifiConnect);
	wifiGotIPHandler = WiFi.onStationModeGotIP(onWifiGotIP);
	wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

	WiFi.setAutoReconnect(true);
	WiFi.persistent(true);       // reconnect to prior access point
	WiFi.begin(ssid, password);

	sp(F("We're also going to wait until WL_CONNECTED"));
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		spl(F("Conn. fail! Rebooting..."));
		delay(3000);
		ESP.restart();
	}
	WiFi.setAutoReconnect(true);
	WiFi.persistent(true);       // reconnect to prior access point
}

void onWifiGotIP(const WiFiEventStationModeGotIP& event) {
	Serial.println(F("EVENT: IP established sucessfully."));
	Serial.print(F("IP address: "));
	Serial.println(WiFi.localIP());
	wifi_connflags = WIFI_FLAG_CONNECTED;
}


void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
	Serial.println(F("EVENT: Disconnected from Wi-Fi. Auto-reconnect should happen."));
	/* WiFi.disconnect(); */
	/* WiFi.begin(ssid, password); */
	wifi_connflags = 0;
}

void onWifiConnect(const WiFiEventStationModeConnected& event) {
	long rssi = WiFi.RSSI();
	Serial.print(F("EVENT: Connected to Wi-Fi sucessfully. Strength: "));
	Serial.println(rssi);
}

// Optional call to use if trying to requiring wifi during setup()
// Wait max of passed seconds for wifi
// Returns flags immediately upon success (eg. WIFI_FLAG_CONNECTED)
// Return flags of 0 means NOT connected for timeout period
uint16_t setup_wait_wifi(unsigned int timeout_s) {
	unsigned long mil = millis();
	bool ret;
	while (((millis() - mil)/1000) < timeout_s) {
		ret = loop_check_wifi(); // after 3s this fn will start printing to serial
		if (ret) return ret;
		delay(99);
	}
	return 0;
}

uint16_t loop_check_wifi() {
	static int connected=false;
	unsigned long cur_millis = millis();
	static unsigned long last_wifi_millis = cur_millis;
	static unsigned long last_connect_millis = 0;
	static unsigned long last_reconnect_millis = 0;
	if (cur_millis - last_wifi_millis < 2000) {
		return WIFI_FLAG_IGNORE;
	} else {
		last_wifi_millis = cur_millis;
		if (WiFi.status() == WL_CONNECTED) {
			if (!connected) { // only if we toggled state
				connected = true;
				last_connect_millis = cur_millis;
				sp(F("Just connected to "));
				sp(ssid);
				sp(F(". IP: "));
				spl(WiFi.localIP());
				WiFi.setAutoReconnect(true);
				WiFi.persistent(true);       // reconnect to prior access point
				return (WIFI_FLAG_CONNECTED | WIFI_FLAG_RECONNECTED);
			} else {
				return WIFI_FLAG_CONNECTED;
			}
		} else {
			if (!connected) {
				#ifndef PLOT_TO_SERIAL
					sp(F("Not connected to wifi. millis="));
					sp(cur_millis);
					sp(F(", cur-last="));
					spl(cur_millis - last_wifi_millis);
				#endif
				if (cur_millis - last_reconnect_millis > MAX_MS_BEFORE_RECONNECT) {
					#ifndef PLOT_TO_SERIAL
						spl(F("  Not connected to WiFi. Reconnecting (disabled)"));
					#endif
					last_reconnect_millis = cur_millis;
					// WiFi.reconnect();
				}
			} else { // only if we toggled state
				connected=false;
				spl(F("Lost WiFi connection. Will try again."));
			}
		}
	}
	return 0; // not connected
}

