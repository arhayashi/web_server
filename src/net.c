#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>

# define PORT "3000"

int main() {
    struct addrinfo hints;
    struct addrinfo *res;

    int status;

    /* clear so garbage data doesn't interfere with response */

    memset(&hints, 0, sizeof(hints));

    /* initialize hints to limit responses */

    hints.ai_flags = AI_PASSIVE;        /* use loopback address */
    hints.ai_family = AF_UNSPEC;        /* use IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(NULL, PORT, &hints, &res);

    if (status != 0) {
        fprintf(stderr, "error while retrieving address info: %s\n",
                gai_strerror(status));
    }

    printf("status: %d\n", status);

}
