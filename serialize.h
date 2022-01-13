#include <ctype.h>

void packi16(uint8_t *buf, uint16_t i);
void packi32(uint8_t *buf, uint32_t i);
void packi64(uint8_t *buf, uint64_t i);
int16_t unpacki16(uint8_t *buf);
uint16_t unpacku16(uint8_t *buf);
int32_t unpacki32(uint8_t *buf);
uint32_t unpacku32(uint8_t *buf);
int64_t unpacki64(uint8_t *buf);
uint64_t unpacku64(uint8_t *buf);
