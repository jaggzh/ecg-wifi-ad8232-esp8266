#ifndef _MAGICBUF_H
#define _MAGICBUF_H
#define MAXBUFBYTES 10000000  // if no magic found, limit buffer

struct magicbuf {
	uint8_t *b;
	int32_t nexti;
	const uint8_t *st_mag, *en_mag;
	uint16_t st_mag_sz, en_mag_sz;
	uint32_t maxlen;
	uint32_t bsize;
	uint32_t maxbuf;
	int (*free)(struct magicbuf *);
	int (*add)(struct magicbuf *, uint8_t *buf, uint32_t blen);
	void (*cb_block)(uint8_t *buf, uint32_t blen);
};

int mbuf_new(struct magicbuf *mb,
		const uint8_t *st_mag,
		uint16_t st_mag_sz,
		const uint8_t *en_mag,
		uint16_t en_mag_sz,
		void (*cb_block)(uint8_t *buf, uint32_t blen)
		);
int _mbuf_mb_add(struct magicbuf *self, uint8_t *buf, uint32_t blen);
int _mbuf_mb_free(struct magicbuf *self);

#endif // _MAGICBUF_H
