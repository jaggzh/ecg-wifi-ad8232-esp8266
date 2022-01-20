#define _GNU_SOURCE   // for asprintf()
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>   // exit()
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>   // va_start() etc (var args)
#include <ctype.h>    // isspace()

#include <unistd.h>   // read/write, stat
#include <sys/types.h> // stat()
#include <sys/stat.h>

#include "settings.h" // copy from setting-example.h
#include "main.h"
#include "server_cbs.h"
#include "serialize.h"
#include "magicbuf.h"
#include "../netdata-settings.h"

struct CMDConnData {
    char login;
    char *datafn;
	FILE *dataf;
	int sockfd;
	struct magicbuf mbuf;
};
struct CMDConnData connst = {
	.login=0, .datafn=NULL, .dataf=NULL, .sockfd=-1
};

int main(int argc, char *argv[]) {
	setup();
	svr_start(PORT); // doesn't end until dead
	return 0;        // probably never gets here
}

void setup() {
  	if (!init_datadir()) {
  		exit(1); // <-- outputs msg on error
  	} else {
		cb_svr_sig = our_cb_svr_sig;         // at termination of server
		cb_svr_cl_connect = our_cb_cl_connect; // for each connect
		cb_svr_cl_read = our_cb_cl_read;       // for each read
		cb_svr_cl_disconnect = our_cb_cl_disconnect; // for each connect
	}
}

void our_cb_svr_sig(int sig) {
	printf("---> CB: Server terminating\n");
}

void our_cb_buf_bundle(uint8_t *buf, uint32_t blen) {
	// Receives buffer.
	// FIRST BYTE IS OUR TYPE FLAG:
	//   #define PAKTYPE_SIZE 1
	//   #define PAKTYPE_TYPE uint8_t
	//   // packet/bundle types:
	//   #define PAK_T_DATA 1
	//   #define PAK_T_BTN  2
	PAKTYPE_TYPE paktype = *buf;
	buf += PAKTYPE_SIZE;   // offset to actual data
	blen -= PAKTYPE_SIZE;  // ^
	if (paktype == PAK_T_DATA) {
		handle_bundle_data(buf, blen);
	} else if (paktype == PAK_T_BTN) {
		handle_bundle_btn(buf, blen);
	}
}
	
void handle_bundle_btn(uint8_t *buf, uint32_t blen) {
	printf("BUTTON Packet Received\n");
}

void handle_bundle_data(uint8_t *buf, uint32_t blen) {
	uint8_t *e = buf+blen;
	uint8_t *s;
	int i;
	for (i=0, s=buf; s < e; i++) {
		uint32_t us;
		uint16_t val;
		us = unpacku32(s);   s += 4;
		val = unpacku16(s);  s += 2;
		//printf("us: %lu  val: %u\n", us, val);
		fprintf(connst.dataf, "%lu %u\n", us, val);
	}
	printf("Wrote %d samples\n", i);
	/* if (!fwrite(buf, blen, 1, connst.dataf)) { */
	/* 	// \/ separate in case some segfault or something */
	/* 	printf("Error writing to data file: "); fflush(stdout); */
	/* 	printf("%s\n", connst.datafn); */
	/* } else { */
	/* 	fflush(connst.dataf); */
	/* } */
}

void our_cb_cl_connect(
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr
		) {
	printf("---> CB: Client connected: IP=%s\n", ipstr);
	connst.sockfd = sockfd;
	mbuf_new(&connst.mbuf,
			stmag,  MAGIC_SIZE,
			enmag,  MAGIC_SIZE,
			our_cb_buf_bundle
			);
}

void sockprintf(char *fmt, ...) {
	char *s;
	if (connst.sockfd) {
		va_list ap;
		va_start(ap, fmt);

		vasprintf(&s, fmt, ap);
		write(connst.sockfd, s, strlen(s));
		free(s);
		va_end(ap);
	}
}

void our_cb_cl_read(                // called on data read
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr,
		char *buf,
		size_t buflen
		) {
	printf("---> CB: Our read callback called\n");
	if (!connst.login) {
		char s_login[]="login 123456"; // check full line
		int rc;
		char *s;
		/* sockprintf("Memcpy=%d\n", memcmp(buf, s_login, sizeof(s_login))); */
		/* sockprintf("   last char=%d\n", buf[buflen-1]); */
		/* sockprintf("last char -1=%d\n", buf[buflen-2]); */
		/* sockprintf("    end char=%d\n", buf[sizeof(s_login)-1]); */
		if (buflen >= sizeof(s_login)-1
				&& !memcmp(buf, s_login, sizeof(s_login)-1) // no NUL
				&& isspace(buf[sizeof(s_login)-1])) {       // fail -->
			char s_logingood[]="Logged in\n";
			write(sockfd, s_logingood, sizeof(s_logingood)-1);
			connst.login = 1;
			printf("User logged in successfully\n");
			if (!begin_login_session()) {
				printf("Error beginning login session\n");
				exit(1);
			}
		} else {                                      // success
			char s_loginfail[]="Not logged in\n";
			write(sockfd, s_loginfail, sizeof(s_loginfail)-1);
			printf("Not logged in\n");
		}
	} else {
		process_ip_packet(buf, buflen);
	}
}

// \/  called for each read after user login
void process_ip_packet(char *buf, int buflen) {
	connst.mbuf.add(&connst.mbuf, buf, buflen);
	return;
}

void our_cb_cl_disconnect(                // called on data read
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr,
		int readval
		) {
	printf("---> CB: Our client disconnected with read() = %d\n", readval);
	end_login_session();
}

// Other local routines

int init_datadir() {
  	struct stat statf;
	int res;
  	if ((res = mkdir(DATA_DIR, 0700))) {
  		if (errno != EEXIST) {
			printf("ERROR CREATING DATA DIR: %s\n", DATA_DIR);
			return 0;
		}
	}
	if (access(DATA_DIR, W_OK)) {
		printf("wsserver: ERROR ACCESSING DATA DIR: %s\n", DATA_DIR);
    	return 0;
	}
	printf("Using data directory: %s\n", DATA_DIR);
	return 1; // success
}

// returns 0 on error (ie. couldn't create data file)
char begin_login_session() {
	connst.datafn = strdup(make_data_fn()); // static storage, not thread safe
	if (!(connst.dataf = fopen(connst.datafn, "a"))) {
		printf("ERROR OPENING DATA FILE: %s\n", connst.datafn);
		return 0;
	}
	printf("Opened data session file: %s\n", connst.datafn);
	return 1;
}

char end_login_session() {
	if (connst.dataf) {
		fclose(connst.dataf);
		connst.dataf = NULL;
	}
	if (connst.datafn) { // must free
		free(connst.datafn);
		connst.datafn = NULL;
	}
	connst.login = 0;
	connst.sockfd = -1;
}

char *make_data_fn() {
	time_t rawtime;
	struct tm *timeinfo;
	static char buffer[512];
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	strftime(buffer, 511, DATA_DIR "/" "%Y-%m-%d--%H:%M%p", timeinfo);
	return buffer;
}

