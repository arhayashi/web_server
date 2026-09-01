#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http.h"
#include "net.h"

int main() {
    int server_socket = get_server_socket();
    printf("server socket: %d\n", server_socket);

    while (1) {
        int client_socket = get_client_socket(server_socket);
        printf("client socket: %d\n", client_socket);

        handle_http_request(client_socket);

        close(client_socket);
    }

    close(server_socket);
}
