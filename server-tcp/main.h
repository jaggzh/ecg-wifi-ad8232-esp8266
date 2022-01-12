#include <netdb.h>
#include <netinet/in.h>

// Matches client .ino
/* struct ecg_packet { */
/* 	uint32_t us; */
/* 	uint16_t val; */
/* }; */
/* #define MAX_PACKETS     (1501/sizeof(struct ecg_packet)) */
/* #define MAX_PACKETS_LEN (MAX_PACKETS * sizeof(struct ecg_packet)) */

// Matches client .ino
#define PAK_SIZE (4+2)
#define MAX_PACKETS  (400/PAK_SIZE) // fit within ESP's packet (524?)
char ecg_netdata[(4+2)*MAX_PACKETS];


void setup();
void our_cb_svr_sig(int sig);
void our_cb_cl_connect(
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr
		);
void our_cb_cl_read(
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr,
		char *buf,
		size_t buflen
		);
void our_cb_cl_disconnect(
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr,
		int readval
		);

// Other local routines

int init_datadir();
char begin_login_session(); // call on successful login
char end_login_session();

// \/  called for each read after user login
void process_user_data(char *buf, int buflen);

char *make_data_fn();       // creates data/log filename in static buf
void sockprintf(char *fmt, ...);  // prints to sockfd
