/// magic buffer!
#define _GNU_SOURCE
#include <string.h>  // memmem
#include <stdint.h>  // *int*_t
#include <stdlib.h>
#include <stdio.h>

#include "magicbuf.h"
#define NETDATA_SETTINGS_MAIN
#include "../netdata-settings.h"

#define MBUF_INITSIZE 32

int mbuf_new(struct magicbuf *mb,
		const uint8_t *st_mag,
		uint16_t st_mag_sz,
		const uint8_t *en_mag,
		uint16_t en_mag_sz,
		void (*cb_block)(uint8_t *buf, uint32_t blen)
		) {
	#if MBUF_INITSIZE < 1
		#error "MBUF_INITSIZE has to be a positive byte count for now."
	#endif
	if (!(mb->b = calloc(1, MBUF_INITSIZE))) {
		perror("Can't allocate magic buffer!");
		exit(1);
	}
	mb->bsize = MBUF_INITSIZE;
	mb->nexti=0;
	mb->st_mag=st_mag;
	mb->st_mag_sz=st_mag_sz;
	mb->en_mag=en_mag;
	mb->en_mag_sz=en_mag_sz;
	mb->free = _mbuf_mb_free;
	mb->add = _mbuf_mb_add;
	mb->cb_block = cb_block;
	mb->maxbuf = MAXBUFBYTES;
	return 0;
}

int _mbuf_mb_add(struct magicbuf *self, uint8_t *buf, uint32_t blen) {
	// If buffer out of room, and we're at the maxbuf size,
	// shift data (results in loss of data):
	if (self->nexti + blen > self->maxbuf) {
		if (blen > self->maxbuf) {
			printf("ERROR: Dropping too big incoming data (%d > %d maxbuf)",
					blen, self->maxbuf);
			return 1; // just ignore it. later maybe we keep the end
		}
		printf("ERROR: Buffer's grown too large (%d is maxbuf).\n"
		       "   For now we're dumping all old data.\n",
		       self->maxbuf);
		// TODO: shift by enough for new data
		// TODO: Must ensure we're using the current buffer size,
		//       (ie. not assuming there's room for blen data)
		// memmove(self->b, self->b + blen, self->nexti - blen);
		self->nexti = 0; // reset End of Data position
	}
	if (blen + self->nexti > self->bsize) {
		self->bsize = blen + self->nexti;
		self->b = realloc(self->b, self->bsize);
		if (!self->b) {
			perror("Realloc failed");
			exit(1);
		}
	}
	memcpy(self->b + self->nexti, buf, blen);
	self->nexti += blen;
	uint8_t *s, *e;
	// look for start magic in entire buffer [0 .. nexti]
	/* printf("Buffer now looks like:\n{{"); */
	/* fwrite(self->b, self->nexti, 1, stdout); */
	/* printf("}}\n"); */
	s=memmem(self->b, self->nexti, self->st_mag, self->st_mag_sz);
	if (s) {        // look for end magic [s .. nexti] = [s ..  nexti-(s-b)]
		s += self->st_mag_sz;
		e = memmem(s + self->st_mag_sz,
				self->nexti - (s-self->b) - self->st_mag_sz,
				self->en_mag, self->en_mag_sz);
		if (e) {    // end found
			/* printf("Found a block! %d bytes.\n{{", e-s); */
			/* fwrite(s, e-s, 1, stdout); */
			/* printf("}}\n"); */
			if (self->cb_block) {
				(*(self->cb_block))(s, e-s);
			}
			// Block handled. Relocate remaining to the start of the buffer
			e += self->en_mag_sz;
			self->nexti -= (e - self->b);
			memmove(self->b, e, self->nexti);
		}
	}
	return 0;
}

int _mbuf_mb_free(struct magicbuf *self) {
	if (self->b) free(self->b);
	return 0;
}

#ifdef MAGICBUF_TEST_MAIN
int main() {
	struct magicbuf mbuf, *mb=&mbuf; // funner to access with ->
	mbuf_new(mb, stmag, sizeof(stmag), enmag, sizeof(stmag));
	mb->add(mb, "Hello ", 5);
	mb->add(mb, MAGIC_ST_STR "World" MAGIC_EN_STR, 5 + sizeof(stmag) + sizeof(enmag));
	mb->add(mb, "Hello ", 6);
	mb->add(mb, MAGIC_ST_STR "World", sizeof(stmag)+5);
	mb->add(mb, MAGIC_EN_STR MAGIC_ST_STR ". Anew", sizeof(enmag)+sizeof(stmag)+6);
	mb->add(mb, ", indeed." MAGIC_EN_STR, 9 + sizeof(enmag));
}
#endif
