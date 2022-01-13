CFLAGS=-O3         # Either optimized or gdb debug
CFLAGS=-ggdb3

all: magicbuf_test

#all: server_cbs

clean:
	rm server_cbs

server_cbs: server_cbs.c server_cbs.h main.c main.h settings.h \
		serialize.c serialize.h
	gcc $(CFLAGS) -std=gnu17 -o server_cbs \
		main.c server_cbs.c serialize.c \
		-pthread

magicbuf_test: magicbuf.c magicbuf.h
	gcc $(CFLAGS) -std=gnu17 -DMAGICBUF_TEST -o magicbuf_test magicbuf.c && ./magicbuf_test

tags:
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
		serialize.c \
		serialize.h \
		../server-websockets/server.cc \
		settings-example.h \
		settings.h
