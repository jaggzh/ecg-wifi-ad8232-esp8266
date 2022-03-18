#include <Arduino.h>
#include <ESP8266WiFi.h>

#define WIFI_FLAG_CONNECTED   0x01
#define WIFI_FLAG_RECONNECTED 0x02
#define WIFI_FLAG_IGNORE      0x04

#ifndef _IN_WIFI_CPP
extern uint16_t wifi_connflags;
#endif

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
void onWifiConnect(const WiFiEventStationModeConnected& event);
void onWifiGotIP(const WiFiEventStationModeGotIP& event);

void setup_wifi(void);

 // Can be called any time, but it won't do any processing except after 3s,
 // and every 3s thereafter.  (Pass it the
 // the passed time and millis() it will start to
 // print status.
 // It does not wait or delay.
 // There's no need to call this a billion times so you
 // Returns:
 //   true if connected
 //   false if not yet connected
 // Outputs to serial on connect. Usually call in loop()
 // pass it the current millis as of that loop
 // It's not necessary -- it doesn't actually do anything
 // except let you see the SSID and IP upon connect
 //
 // Once connected it will remember and won't output
 // again until the status changes.
uint16_t loop_check_wifi();  // optional, for connection status Serial output
void loop_wifi();        // Required for loop updates

// Optional call to use if trying to requiring wifi during setup()
// Wait max of passed seconds for wifi
// Returns flags immediately upon success (eg. WIFI_FLAG_CONNECTED)
// Return flags of 0 means NOT connected for timeout period
uint16_t setup_wait_wifi(unsigned int timeout_s=10);
