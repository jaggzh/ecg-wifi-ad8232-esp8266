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

struct magicbuf {
	uint8_t *b;
	int32_t nexti;
	const uint8_t *st_mag, *en_mag;
	uint16_t st_mag_sz, en_mag_sz;
	uint32_t maxlen;
	uint32_t bsize;
	int (*free)(struct magicbuf *);
	int (*add)(struct magicbuf *, uint8_t *buf, uint32_t blen);
	void (*cb_block)(struct magicbuf *, uint8_t *buf, uint32_t blen);
};

int mbuf_new(struct magicbuf *mb,
		const uint8_t *st_mag,
		uint16_t st_mag_sz,
		const uint8_t *en_mag,
		uint16_t en_mag_sz
		);
int _mbuf_mb_add(struct magicbuf *self, uint8_t *buf, uint32_t blen);
int _mbuf_mb_free(struct magicbuf *self);