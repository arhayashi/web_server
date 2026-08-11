#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT "3000"
#define BACKLOG (10)

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
        freeaddrinfo(res);
        exit(1);
    }

    /* enable to make another socket with the same address after termination */

    int on = 1;
    status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (status == -1) {
        fprintf(stderr, "server error : unable to set socket options\n");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }

    /* bind socket to address */

    status = bind(sockfd, res->ai_addr, res->ai_addrlen);

    if (status == -1) {
        fprintf(stderr, "server error : unable to bind socket\n");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }

    status = listen(sockfd, BACKLOG);

    if (status == -1) {
        fprintf(stderr, "server error : unable to listen with socket\n");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }
   
    struct sockaddr_storage cli_sockaddr;
    socklen_t cli_addrlen;
    int cli_sockfd;
    
    cli_sockfd = accept(sockfd, (struct sockaddr *)&cli_sockaddr,
                        &cli_addrlen);

    if (cli_sockfd == -1) {
        fprintf(stderr, "server error : unable to accept socket\n");
        close(sockfd);
        freeaddrinfo(res);
        exit(1);
    }
    
    printf("client socket: %d\n", cli_sockfd);

    close(sockfd);
    close(cli_sockfd);
    freeaddrinfo(res);

}
