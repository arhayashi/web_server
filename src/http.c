/*
 * This file contains code to deal with HTTP requests and responses, including
 * parsing request values and forming responses.
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "http.h"

#define BUFF_LEN (65536)     /* max length of HTTP req, res (64k bytes) */
#define METHOD_LEN (20)      /* max length of HTTP method  */
#define RESOURCE_LEN (1024)  /* max length of request resource */
#define REQ_VALS_CT (2)      /* num of vals to be extracted from valid req */

/*
 * This function extracts the request's method and desired resource. On 
 * success, this function returns zero. Invalid extractions return a non-zero
 * value to indicate a malformed request.
 */

int parse_http_request(char *request, char *method, char *resource) {
    int status;

    /* extract request's method and desired resource */

    status = sscanf(request, "%19[^ ] %1023[^ ]", method, resource);

    /* zero if successfully extracted both values */

    return status - REQ_VALS_CT;
} /* parse_http_request() */

/*
 * Handles the HTTP request from the client
 */

void handle_http_request(int client_socket) {
    char request[BUFF_LEN];  /* holds entirety of client's HTTP request */
    memset(request, 0, BUFF_LEN);

    int recv_bytes = recv(client_socket, request, BUFF_LEN - 1, 0);

    if (recv_bytes == -1) {
       fprintf(stderr, "[ERROR] unable to read HTTP request\n"); 
    }

    if (recv_bytes == 0) {  /* indicates client closed connection */
        return;
    }

    request[recv_bytes] = '\0'; 

    char method[METHOD_LEN];      /* stores HTTP method ie. GET, POST */
    char resource[RESOURCE_LEN];  /* stores the requested resource */

    int status = parse_http_request(request, method, resource);

    if (status != 0) {  /* indicates malformed request */
        // TODO: send 400 response
    }

    handle_http_response(client_socket, method, resource);
} /* handle_http_request() */

/*
 * Test
 */

void handle_http_response(int client_socket, char *method, char *resource) {
    printf("client sock: %d\n", client_socket);
    printf("method: %s\n", method);
    printf("resource: %s\n", resource);
} /* handle_http_response() */
