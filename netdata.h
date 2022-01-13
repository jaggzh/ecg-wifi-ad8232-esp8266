#ifndef _NETDATA_H
#define _NETDATA_H

#define MAGICBUF_TEST

#define US_NETDATA_TEST 10000         // between testing connection

// 'Magic' start/end sequences surrounding sensor bundles
// * array versions are used so sizeof() works directly
// * string versions are included mostly only for tests
//   where you might want to concatenate them (see bottom of
//   magicbuf.c)
#ifndef MAGICBUF_TEST
#define MAGIC_ST { 0375, 003, 0374 }  // start packet bundle magic
#define MAGIC_EN { 0371, 004, 0372 }  // end
#define MAGIC_ST_STR "\375\003\374"   // Some might like to use this
#define MAGIC_EN_STR "\371\004\372"   //  in string concatenation
#else
#define MAGIC_ST { 0101, 0132, 0102 }  // start packet bundle magic
#define MAGIC_EN { 0104, 0131, 0103 }  // end
#define MAGIC_ST_STR "AZB"   // Some might like to use this
#define MAGIC_EN_STR "DYC"   //  in string concatenation
#endif

void setup_netdata(); // once in setup()
void loop_netdata();  // each loop() probably.. maybe.
void netdata_add(uint16_t v); // call to add value to send
void netdata_send();

#endif // _NETDATA_H
