#ifndef _NETDATA_H
#define _NETDATA_H

#define US_NETDATA_TEST 10000
#define MAGIC_ST "\375\003\374"  // start packet bundle magic
#define MAGIC_EN "\371\004\372"  // end

void setup_netdata(); // once in setup()
void loop_netdata();  // each loop() probably.. maybe.
void netdata_add(uint16_t v); // call to add value to send
void netdata_send();

#endif // _NETDATA_H
