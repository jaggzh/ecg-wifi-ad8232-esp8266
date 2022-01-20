CFLAGS=-O3         # Either optimized or gdb debug
CFLAGS=-ggdb3

all: server_cbs

tests: magicbuf_test

clean:
	rm server_cbs magicbuf_test

server_cbs: server_cbs.c server_cbs.h main.c main.h settings.h \
		serialize.c serialize.h magicbuf.c magicbuf.h
	gcc $(CFLAGS) -std=gnu17 -o server_cbs \
		main.c server_cbs.c serialize.c \
		magicbuf.c \
		-pthread

magicbuf_test: magicbuf.c magicbuf.h
	gcc $(CFLAGS) -std=gnu17 -DMAGICBUF_TEST_MAIN -o magicbuf_test magicbuf.c && ./magicbuf_test

tags: *.c *.h
	ctags *.c *.h

vi:
	vim \
		Makefile \
		server_cbs.c \
		server_cbs.h \
		main.c \
		main.h \
		magicbuf.c \
		magicbuf.h \
		../netdata-settings.h \
		serialize.c \
		serialize.h \
		../server-websockets/server.cc \
		settings-example.h \
		settings.h
