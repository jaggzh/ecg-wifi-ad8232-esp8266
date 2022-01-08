#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define WIFI_CONFIG_GET_IPS
#define __WIFI_CPP
#include "wifi_config.h"
#include "wifi.h"
#include "printutils.h"

void setup_wifi(void) {
	WiFi.mode(WIFI_STA);
	WiFi.config(ip, gw, nm);
	WiFi.begin(ssid, password);
	//sp(F("Connecting to wife..."));
	/* while (WiFi.waitForConnectResult() != WL_CONNECTED) */
	/* 	{ spl(F("Conn. fail! Rebooting...")); delay(5000); ESP.restart(); } */
	WiFi.setAutoReconnect(true);
	WiFi.persistent(true);       // reconnect to prior access point
}

// Wait max of passed seconds for wifi
// Returns true immediately upon success
// False after timeout expires
bool setup_wait_wifi(int timeout_s) {
	int mil = millis();
	bool ret;
	while (((millis() - mil)/1000) < timeout_s) {
		ret = loop_check_wifi(); // after 3s this fn will start printing to serial
		if (ret) return ret;
		delay(1000);
	}
	return false;
}

bool loop_check_wifi() {
	static int connected=false;
	int cur_millis = millis();
	static int last_wifi_millis = cur_millis;
	static int last_connect_millis = 0;
	if (cur_millis - last_wifi_millis > 3000) {
		last_wifi_millis = cur_millis;
		if (WiFi.status() == WL_CONNECTED) {
			if (!connected) { // only if we toggled state
				connected = true;
				last_connect_millis = cur_millis;
				sp(F("Just connected to "));
				sp(ssid);
				sp(". IP: ");
				spl(WiFi.localIP());
				return true;
			}
		} else {
			if (!connected) {
				spl(F("Still not connected"));
				if (cur_millis - last_connect_millis > MAX_MS_BEFORE_RECONNECT) {
					WiFi.reconnect();
				}
			} else { // only if we toggled state
				connected=false;
				spl(F("Lost WiFi connection. Will try again."));
			}
		}
	}
	return false;
}

void onConnectionEstablished() {
		//Serial.println(payload);});
	//client.publish("mytopic/test", "This is a message");
}
