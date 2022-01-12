CFLAGS=-O3         # Either optimized or gdb debug
CFLAGS=-ggdb3

all: server_cbs

clean:
	rm server_cbs

server_cbs: server_cbs.c server_cbs.h main.c main.h settings.h
	gcc $(CFLAGS) -std=gnu17 -o server_cbs main.c server_cbs.c -pthread

vi:
	vim \
		Makefile \
		server_cbs.c \
		server_cbs.h \
		main.c \
		main.h \
		../server-websockets/server.cc \
		settings-example.h \
		settings.h
