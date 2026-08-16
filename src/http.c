/*
 * This file contains code to deal with HTTP requests and responses, including
 * parsing requests and forming responses.
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include "http.h"
#include "mime.h"
#include "process_file.h"

#define DATE_LEN (30)         /* length of date string in HTTP header */
#define METHOD_LEN (20)       /* max length of HTTP method  */
#define REQ_LEN (64 * 1024)   /* max length of HTTP request (64kb) */
#define RES_LEN (100 * 1024)  /* max length of HTTP response (100kb) */
#define RESOURCE_LEN (1024)   /* max length of request resource */
#define REQ_VALS_CT (2)       /* num of vals to be extracted from valid req */

#define SERVER_FILES "./root"

#define HTTP_GET(response, mime, content, size)                          \
        create_http_response(response, "HTTP/1.1 200 OK", mime, content, \
                             size)                                       \

/*
 * This function creates an HTTP response, putting it into the given response
 * argument. The HTTP response starts with the given header line, some
 * additional metadata, and then adds the given content.
 */

int create_http_response(char *response, char *header, char *mime,
                         char *content, int size) {
    int amt = 0;

    /* formats date ie. Sun, 16 Aug 2026 14:48:37 GMT */

    time_t secs = time(NULL);              /* time in secs */
    struct tm *curr_time = gmtime(&secs);  /* time in GMT */
    char date[DATE_LEN] = { '\0' };
    amt += strftime(date, sizeof(date), "%a, %d %b %Y %T GMT", curr_time) + 1;

    printf("size ime format: %d\n", size-1);

    amt += snprintf(response, RES_LEN,
            "%s\n"                  /* HTTP status code */
            "Date: %s\n"            /* current date in specific format */
            "Connection: close\n"   /* tells browser to close TCP connection */
            "Content-Length: %d\n"  /* does not include header */
            "Content-Type: %s\n\n"  /* mime type */
            "%s",
            header, date, size, mime, content) + 1;
} /* create_http_response() */

/*
 * This function extracts the request's method and desired resource. On 
 * success, this function returns zero. Invalid extractions return a non-zero
 * value to indicate a malformed request.
 */

int parse_http_request(char *request, char *method, char *resource) {
    int status;

    /* requested resource starts with / so combine with SERVER_FILES to form */
    /* proper relative path */

    char tmp_resource[RESOURCE_LEN] = { '\0' };
    status = sscanf(request, "%19[^ ] %1023[^ ]", method, tmp_resource);

    /* combined tmp_resource here to form proper relative path */

    snprintf(resource, RESOURCE_LEN, "%s%s", SERVER_FILES, tmp_resource);

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
       return;
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
        file_cont_t *file_cont = read_file_cont(resource);

        printf("%p\n", file_cont);

        if (file_cont == NULL) {  /* indicates server error or file doesn't exist */
            // send server error response maybe make it a macro
            printf("[ERROR] unable to get resource\n");
            return;
        }

        char *mime = get_mime_type(resource);

        // make it return length for send()
        int num = create_http_response(response, "HTTP/1.1 200 OK", mime, 
                             file_cont->content, file_cont->size);
        send(client_socket, response, num, 0);
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
