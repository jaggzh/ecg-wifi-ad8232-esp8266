#ifndef _NETDATA_SETTINGS_H
#define _NETDATA_SETTINGS_H
// ***********************
// ** IMPORTANT
// ***********************
// You need to #define NETDATA_SETTINGS_MAIN at least
// in one place that's including this file, or otherwise
// declare those two char [] arrays: stmag and enmag

// This file is for setting your network MAGIC Sequences
// for the starts/ends of data bundles
// See MAGIC_ST, MAGIC_EN, etc.

// Define this in the .c where you want variables to be declared
//  (Before including this .h)
//  (Ie. do not define in other .c files so the variables
//  end up being externs)
// Example: #define NETDATA_SETTINGS_MAIN
//  You can see, below, the externs


//#define MAGICBUF_TEST

// Slow down how often we test for TCP
// (and the reconnections)
#define US_NETDATA_TEST 100000
#define NET_MAXLEN   1000   // Ideally, the max packet size before your
                            //  MCU will fail on them

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
  // ^ but otherwise the _STR versions are only used in the magicbuf
  //   test main() routine, and aren't needed unless you use them somewhere
#else
#define MAGIC_ST { 0101, 0132, 0102 }  // start packet bundle magic
#define MAGIC_EN { 0104, 0131, 0103 }  // end
//#define MAGIC_ST_STR "AZB"   // Some might like to use this
//#define MAGIC_EN_STR "DYC"   //  in string concatenation
  // ^ but otherwise the _STR versions are only used in the magicbuf
  //   test main() routine, and aren't needed unless you use them somewhere
#endif

#ifdef NETDATA_SETTINGS_MAIN
	uint8_t stmag[]=MAGIC_ST;
	uint8_t enmag[]=MAGIC_EN;
#else
	extern uint8_t stmag[];
	extern uint8_t enmag[];
#endif

//static_assert(sizeof(stmag)!= sizeof(enmag), "Let's keep MAGIC_ST and MAGIC_EN the same length");
//static_assert(sizeof((uint8_t[])MAGIC_ST) != sizeof((uint8_t[])MAGIC_ST), "Let's keep MAGIC_ST and MAGIC_EN the same length");

#define MAGIC_SIZE sizeof((uint8_t [])MAGIC_ST)

// Additional FULL net package info
// Must match client .ino
#define SAMPLE_SIZE (4+2)
#define PAKTYPE_SIZE 1
#define PAKTYPE_TYPE uint8_t

// packet/bundle types:
#define PAK_T_DATA 1
#define PAK_T_BTN  2

#define FULLPAK_SIZE (MAGIC_SIZE*2 + PAKTYPE_SIZE + SAMPLE_SIZE*MAX_PACKETS)
#define MAX_PACKETS  (NET_MAXLEN/SAMPLE_SIZE) // fit within ESP's packet (524?)
// ^ this isn't fully accurate, if we were precise with the NET_MAXLEN,
//   since we don't include the magic and stuff
#define PAKFULL_MAGIC_OFFSET (0)          // start of buffer
#define PAKFULL_TYPE_OFFSET (MAGIC_SIZE)  // right after magic
#define PAKFULL_DATA_OFFSET (MAGIC_SIZE + PAKTYPE_SIZE) // after magic and type

// ^ this is how far into the network package.. where the sensor data begins



#endif // _NETDATA_SETTINGS_H
