#ifndef _NETDATA_H
#define _NETDATA_H

#define US_NETDATA_TEST 10000

void setup_netdata(); // once in setup()
void loop_netdata();  // each loop() probably.. maybe.
void netdata_add(uint16_t v); // call to add value to send
void netdata_send();

#endif // _NETDATA_H
