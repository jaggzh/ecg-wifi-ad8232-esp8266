#ifndef __WIFI_CONFIG_H
#define __WIFI_CONFIG_H

#include <ESP8266WiFi.h>

#define MDNS_NAME "LanDevice"
#define SSID_NAME "YourSSID"
#define SSID_PW   "YourPASSWORD"
// The below might not be implemented
#define WEBUPDATE_USER "webuser"
#define WEBUPDATE_PW   "webpw"

#define MAX_MS_BEFORE_RECONNECT 4500

// Server to connect to for logging
// (this is not our MCU IP. That's below.)
#define DATA_HOST       "192.168.1.4"
#define DATA_ARDUINO_IP (192,168,1,4)
#define DATA_PORT       1234
#define DATA_PATH       "/"

#ifdef __WIFI_CPP
const char *ssid = SSID_NAME;
//#define SSPW {33+22, 3, 62+22, 4+2, 6+129, 0}
char password[] = SSID_PW;

const char *update_user = WEBUPDATE_USER; // HTTP auth user for OTA http update
const char *update_pw = WEBUPDATE_PW";  // HTTP auth password

#else // like: ifndef __MAIN_INO__
extern const char *ssid;
extern char password[];
extern const char *update_user; // HTTP auth user for OTA http update
extern const char *update_pw;  // HTTP auth password
#endif

#ifdef WIFI_CONFIG_GET_IPS
IPAddress ip(192, 168, 1, 14);
IPAddress gw(192, 168, 1, 1);
IPAddress nm(255, 255, 255, 0);
#endif

#endif
