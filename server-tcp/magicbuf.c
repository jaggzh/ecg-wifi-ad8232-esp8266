/// magic buffer!
#define _GNU_SOURCE
#include <string.h>  // memmem
#include <stdint.h>  // *int*_t
#include <stdlib.h>
#include <stdio.h>

#include "magicbuf.h"

#define MBUF_INITSIZE 32

int mbuf_new(struct magicbuf *mb,
		const uint8_t *st_mag,
		uint16_t st_mag_sz,
		const uint8_t *en_mag,
		uint16_t en_mag_sz
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
	mb->cb_block = NULL;
	return 0;
}

int _mbuf_mb_add(struct magicbuf *self, uint8_t *buf, uint32_t blen) {
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
	s=memmem(self->b, self->nexti, self->st_mag, self->st_mag_sz);
	if (s) {        // look for end magic [s .. nexti] = [s ..  nexti-(s-b)]
		s += self->st_mag_sz;
		e = memmem(s + self->st_mag_sz,
				self->nexti - (s-self->b) - self->st_mag_sz,
				self->en_mag, self->en_mag_sz);
		if (e) {    // end found
			printf("Found a block! %d bytes.\n{{", e-s);
			fwrite(s, e-s, 1, stdout);
			e += self->en_mag_sz;
			printf("}}\n");
			// Block handled. Relocate remaining to the start of the buffer
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
uint8_t stmag[]=MAGIC_ST;
uint8_t enmag[]=MAGIC_EN;

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
