#include <netdb.h>
#include <netinet/in.h>

int svr_start(int portno);

void _svr_sighandler(int sig);
void _svr_client_handler(int sock, struct sockaddr_in *cli_addr);
void *get_in_addr(struct sockaddr *sa);


#ifndef _IN_SERVER_CBS_C
extern void (*cb_svr_sig)(int sig); // cleanup on user interrupt
extern void (*cb_svr_cl_connect)(      // when client connects
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr
		);
extern void (*cb_svr_cl_read)(         // called on data read
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr,
		char *buf,
		size_t buflen
		);
extern void (*cb_svr_cl_disconnect)(   // when client disconnects
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr,
		int readval
		);
#endif
