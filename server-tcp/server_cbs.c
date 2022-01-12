#define _IN_SERVER_CBS_C
#include "settings.h"
#include "server_cbs.h"

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include <sys/types.h> // fork
#include <unistd.h> // fork

#include <arpa/inet.h> // inet_ntop()

#include <signal.h>

#define SIGCNTS 3
int _sig_nums[SIGCNTS] = {SIGINT, SIGHUP, SIGTERM};
struct sigaction _old_sigs[SIGCNTS];
	/* ^ Note: We don't currently call the old handlers */

/**************************************/
// Callbacks (CB):

// Caught signal for cleanup.
// HOOK: This does not replace the default behavior, it is
// called before closing socket, and passed the sig signal
// number that was received
void (*cb_svr_sig)(int sig) = NULL;
void (*cb_svr_cl_connect)(               // when client connects
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr
		) = NULL;
void (*cb_svr_cl_read)(
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr,
		char *buf,
		size_t buflen
		) = NULL;
void (*cb_svr_cl_disconnect)(           // when client disconnects
		const char *ipstr,
		int sockfd,
		struct sockaddr_in *cli_addr,
		int readval                     // value from read <= 0
		) = NULL;

// Internal
int _svr_sockfd=-1;
int _svr_fork_pid=-1;

void _svr_sighandler(int sig) {
	int i;
	// loop through signals we catch for shutdown
	for (i=0; i<SIGCNTS && sig != _sig_nums[i]; i++);
	if (i>=SIGCNTS) { // we were called with one of them
		// Call user-set callback for cleanup, if any
		if (cb_svr_sig) (*cb_svr_sig)(sig);
		printf("\nCleaning up and exiting...\n");
		if (_svr_sockfd != -1) {
			if (_svr_fork_pid > 0) { // parent process
				printf("Closing socket fd %d (port %d)\n", _svr_sockfd, PORT);
				close(_svr_sockfd);
				_svr_sockfd = -1;
			}
		}
		printf("\n");
		exit(0);
	}
}

void _setup_signals() {
	struct sigaction new_sigaction;
	new_sigaction.sa_handler = _svr_sighandler;
	sigemptyset(&new_sigaction.sa_mask);
	new_sigaction.sa_flags = 0;
	// Set new signal handlers and store originals
	for (int i=0; i<SIGCNTS; i++) {
		// sigaction(SIGINT, &new_sigaction, &oldhandler); 
		// sigaction(SIGHUP, &new_sigaction, &oldact);
		// sigaction(SIGTERM, &new_sigaction, &oldact);
		sigaction(_sig_nums[i], &new_sigaction, &_old_sigs[i]); 
		/* Note: We don't currently call the old handlers */
	}
}

int svr_start(int portno) {
	int newsockfd, cli_addr_len;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	
	_setup_signals();
	_svr_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int flag=1;
	if (-1 == setsockopt(_svr_sockfd, SOL_SOCKET,
				         SO_REUSEADDR, &flag, sizeof(flag))) {
		perror("setsockopt(...SO_REUSEADDR...) failure");
		exit(1);
	}
	
	if (_svr_sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	
	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	/* Now bind the host address using bind() call.*/
	if (bind(_svr_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}
	
	/* Now start listening for the clients, here
		* process will go in sleep mode and will wait
		* for the incoming connection
	*/
	
	listen(_svr_sockfd,5);
	cli_addr_len = sizeof(cli_addr);

	printf("Server started on port %d\n", PORT);
	
	while (1) {
		newsockfd = accept(_svr_sockfd, (struct sockaddr *) &cli_addr, &cli_addr_len);
		printf("Client connected\n");
		
		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}
		
		/* Create child process */
		_svr_fork_pid = fork();
		
		if (_svr_fork_pid < 0) {
			perror("ERROR on fork");
			exit(1);
		}
		
		if (_svr_fork_pid == 0) {
			close(_svr_sockfd);
			/* This is the client process */
			_svr_client_handler(newsockfd, &cli_addr);
			exit(0);
		}
		else {
			close(newsockfd);
		}
		
	} /* end of while */
}

void _svr_client_handler(int sock, struct sockaddr_in *cli_addr) {
	int n;
	char buffer[MAXBUF+1];
	struct addrinfo *p;
	char ipstr[INET6_ADDRSTRLEN];
	/* const char *inet_ntop(int af, const void *src, */
	/* 					char *dst, socklen_t size); */
	printf("_svr_client_handler()\n"); fflush(stdout);
	if (!inet_ntop(AF_INET,
			get_in_addr((struct sockaddr *)cli_addr),
			ipstr, sizeof ipstr)) {
		perror("inet_ntop() returned NULL");
		exit(0);
	}

	printf("Client connected on %s\n", ipstr); fflush(stdout);
	if (cb_svr_cl_connect) {
		(*cb_svr_cl_connect)(
			ipstr,      // char ip_addr_string[]
			sock,       // int sockfd
			cli_addr    // struct sockaddr_in *cli_addr
			);
	}
	while (1) {
		bzero(buffer,MAXBUF);
		n = read(sock,buffer,MAXBUF);
		
		if (n < 0) {
			perror("ERROR reading from socket");
			if (cb_svr_cl_disconnect) {
				(*cb_svr_cl_disconnect)(
					ipstr,      // char ip_addr_string[]
					sock,       // int sockfd
					cli_addr,   // struct sockaddr_in *cli_addr
					n           // read value < 0
					);
			}
			exit(1);
		}
		if (n == 0) {
			if (cb_svr_cl_disconnect) {
				(*cb_svr_cl_disconnect)(
					ipstr,      // char ip_addr_string[]
					sock,       // int sockfd
					cli_addr,   // struct sockaddr_in *cli_addr
					n           // read value < 0
					);
			}
			printf("Client disconnected (EOF on socket)\n");
			exit(0);
		}
		
		printf("Received from %s: %d bytes: {%*s}\n",
			ipstr, n, n, buffer);

		if (cb_svr_cl_read) {
			(*cb_svr_cl_read)(
				ipstr,      // char ip_addr_string[]
				sock,       // int sockfd
				cli_addr,   // struct sockaddr_in *cli_addr
				buffer,     // char buffer[MAXBUF]
				n           // int  buffer length
				);
		}

		/* n = write(sock,"I got your message", 18); */
		/* if (n < 0) { */
		/* 	perror("ERROR writing to socket"); */
		/* 	exit(1); */
		/* } */
	}
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

