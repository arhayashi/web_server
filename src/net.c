#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "3000"

int main() {
    struct addrinfo hints;
    struct addrinfo *res;

    int status;  /* track return values for errors */

    /* clear so garbage data doesn't interfere with response */

    memset(&hints, 0, sizeof(hints));

    /* initialize hints to limit responses */

    hints.ai_flags = AI_PASSIVE;        /* use loopback address */
    hints.ai_family = AF_UNSPEC;        /* use IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(NULL, PORT, &hints, &res);

    if (status != 0) {
        fprintf(stderr, "server err : %s\n", gai_strerror(status));
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sockfd == -1) {
        fprintf(stderr, "server error : unable to create socket\n");
        exit(1);
    }

    status = bind(sockfd, res->ai_addr, res->ai_addrlen);

    if (status == -1) {
        fprintf(stderr, "server error : unable to bind socket\n");
        exit(1);
    }

    int on = 1;

    /* enable to make another socket with the same address after termination */

    status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (status == -1) {
        fprintf(stderr, "server warning : unable to set socket options\n");
    }

    freeaddrinfo(res);
}
