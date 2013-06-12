#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#define PORT       25000
#define BUF_SIZ    4

int main (int argc, char **argv) {

    int sock;
    struct sockaddr_in saddr, caddr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
   // uint8_t buf[BUF_SIZ];
   //double buf[BUF_SIZ];
   int16_t buf[BUF_SIZ];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
	perror("ERROR:");
	exit (-1);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY); //server receives packets from all interfaces
					        
    saddr.sin_port = htons(PORT);

    if (bind(sock,(struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
	perror("ERROR:");
	exit (-1);
    }

    while(1) {
	memset(buf, 0, sizeof(buf));

	if(recvfrom(sock,
		    &buf,
		    sizeof(buf),
		    0,
		    (struct sockaddr *) &caddr, 

		    &addrlen) < 0) {
	    perror("ERROR:");
	    exit (-1);
	}

//printf("%.f %.f %.f\n", buf[0], buf[1], buf[2]);
printf("%d %d %d %d\n", buf[0], buf[1], buf[2], buf[3]);
	    
    }

    return 1;
}
