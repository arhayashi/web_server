/*
 * This file contains code to deal with HTTP requests and responses, including
 * parsing request values and forming responses.
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include "http.h"

#define DATE_LEN (30)         /* length of date string in HTTP header */
#define REQ_LEN (64 * 1024)   /* max length of HTTP request (64kb) */
#define RES_LEN (100 * 1024)  /* max length of HTTP response (100kb) */
#define METHOD_LEN (20)       /* max length of HTTP method  */
#define RESOURCE_LEN (1024)   /* max length of request resource */
#define REQ_VALS_CT (2)       /* num of vals to be extracted from valid req */

/*
 * Header
 */

void create_http_response(char *response, char *header) {
    time_t secs = time(NULL);              /* time in secs */
    struct tm *curr_time = gmtime(&secs);  /* time in GMT */
    char date[DATE_LEN] = { '\0' };
    strftime(date, sizeof(date), "%a, %d %b %Y %T GMT", curr_time);

    sprintf(response, "%s\n%s\n", header, date);
} /* create_http_response() */

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
    char request[REQ_LEN] = { '\0' };  /* holds client's HTTP request */

    int recv_bytes = recv(client_socket, request, REQ_LEN - 1, 0);

    if (recv_bytes == -1) {
       fprintf(stderr, "[ERROR] unable to read HTTP request\n"); 
    }

    if (recv_bytes == 0) {  /* indicates client closed connection */
        return;
    }

    request[recv_bytes] = '\0'; 

    char method[METHOD_LEN] = { '\0' };      /* HTTP method ie. GET, POST */
    char resource[RESOURCE_LEN] = { '\0' };  /* requested resource */

    int status = parse_http_request(request, method, resource);

    handle_http_response(client_socket, method, resource, status);
} /* handle_http_request() */

/*
 * Test
 */

void handle_http_response(int client_socket, char *method, char *resource,
                          int status) {
    char response[RES_LEN] = { '\0' };

    if (status != 0) {  /* indicates malformed request */
        // TODO: send 400 response
    }

    if (strcmp("GET", method) == 0) {
        create_http_response(response, "HTTP/1.1 200 OK");
    } else if (strcmp("POST", method) == 0) {

    } else {
        // 501 - not implemented
    }

    printf("client sock: %d\n", client_socket);
    printf("method: %s\n", method);
    printf("resource: %s\n", resource);
    printf("whole thing...\n");
    printf("%s\n", response);
} /* handle_http_response() */
