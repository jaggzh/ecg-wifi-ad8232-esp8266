#ifndef _NETDATA_H
#define _NETDATA_H
#include <stdint.h> // for uint8_t etc.
#include <assert.h> // for static_assert(), for sizeof() in #if

void setup_netdata(); // once in setup()
void loop_netdata();  // each loop() probably.. maybe.
void netdata_add(uint16_t v); // call to add value to send
void netdata_send();
void netdata_send_btn1();

#endif // _NETDATA_H
