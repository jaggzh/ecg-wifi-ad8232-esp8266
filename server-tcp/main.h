#include <netdb.h>
#include <netinet/in.h>

// Matches client .ino
/* struct ecg_packet { */
/* 	uint32_t us; */
/* 	uint16_t val; */
/* }; */
/* #define MAX_PACKETS     (1501/sizeof(struct ecg_packet)) */
/* #define MAX_PACKETS_LEN (MAX_PACKETS * sizeof(struct ecg_packet)) */

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
// \/ Final processor of a received bundle of data:
void our_cb_buf_bundle(uint8_t *buf, uint32_t blen);
// \/ Called by our_cb_buf_bundle() when data type packet received
void handle_bundle_data(uint8_t *buf, uint32_t blen);
void handle_bundle_btn1(uint8_t *buf, uint32_t blen);

// Other local routines

int init_datadir();
char begin_login_session(); // call on successful login
void end_login_session();

char begin_file_session();  // creates and opens file
void end_file_session();

// \/  called for each read after user login
void process_ip_packet(char *buf, int buflen);

char *make_data_fn();       // creates data/log filename in static buf
void sockprintf(char *fmt, ...);  // prints to sockfd
