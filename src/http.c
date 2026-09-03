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

#define DATE_LEN    (30)         /* length of date string in HTTP header */
#define METHOD_LEN  (20)         /* max length of HTTP method  */
#define REQ_LEN     (64 * 1024)  /* max length of HTTP request (64kb) */
#define RES_LEN     (100 * 1024) /* max length of HTTP response (100kb) */
#define TARGET_LEN  (1024)       /* max length of request's target */
#define REQ_VALS_CT (2)          /* num of vals to be extrctd from valid req */

#define SERVER_FILES "./root"
#define DEFAULT_FILE "/index.html"
#define FILE_404     SERVER_FILES "/404.html"

#define SERVER_ERR  (-1)
#define SERVER_WARN (0)
#define SERVER_SUCC (1)

/*
 * This function verifies that an HTTP response was successfully created,
 * returning status if it was. If it wasn't, this function attempts to
 * create a HTTP 500 response and put it into the response argument. If that
 * wasn't successfully created, a critical error occured and the function
 * returns SERVER_ERR. If the HTTP 500 response was successfully created, then
 * status is returned, which is now the size of the HTTP 500 response. The
 * status argument contains the return value of the HTTP response that was
 * attempted to be created and is being error checked.
 */

int check_http_res(char *response, int status) {
    if (status != SERVER_ERR) {
        return status;
    }

    status = http_500(response);

    if (status == SERVER_ERR) {
        fprintf(stderr, "[ERROR] could not create an HTTP 500 Internal Server "
                        "Error response...critical error\n");
        return SERVER_ERR;
    }

    fprintf(stderr, "[WARNING] response now contains an HTTP 500 Internal "
                    " Server Error response\n");

    return status;
} /* check_http_res() */

/*
 * Creates a HTTP 200 OK response, putting it into the response argument.
 */

int http_200(char *response, char *target, file_cont_t **file_cont) {
    int status;
    char *header = "HTTP/1.1 200 OK";

    *file_cont = read_file_cont(target, &status);

    /* HTTP 200 OK if file exists */

    if (*file_cont != NULL) {
        printf("[LOG] sending HTTP 200 OK response\n");

        char *mime = get_mime_type(target);
        return create_http_response(response, header, mime,
                                    (*file_cont)->content,
                                    (*file_cont)->size);
    } 

    /* try again with .../index.html if requested target is a directory */

    if (status == FILE_DIR) {
        int remaining_size = (TARGET_LEN - strlen(target)) - 1;
        char *new_target = strncat(target, DEFAULT_FILE, remaining_size);

        printf("[LOG] attempting to serve %s\n", new_target);

        *file_cont = read_file_cont(new_target, &status);

        /* on success, send HTTP 200 OK */

        if (*file_cont != NULL) {
            printf("[LOG] sending HTTP 200 OK response\n");

            char *mime = get_mime_type(new_target);
            return create_http_response(response, header, mime,
                    (*file_cont)->content,
                    (*file_cont)->size);
        }

        /* otherwise, send HTTP 404 Not Found */

        if (status == FILE_NEXS) {
            return http_404(response, FILE_404, file_cont);
        }

        /* HTTP 500 Internal Server Error */

        return SERVER_ERR;
    }

    if (status == FILE_NEXS) {
            return http_404(response, FILE_404, file_cont);
    }

    if (status == FILE_ERR) {
        return SERVER_ERR;
    }

    return SERVER_SUCC;
} /* http_200() */

/*
 * Creates a HTTP 400 Bad Request response, putting it into the response
 * argument. 
 */

int http_400(char *response) {
    fprintf(stderr, "[WARNING] sending HTTP 400 Bad Request response\n");

    char *header = "HTTP/1.1 400 Bad Request";
    char *content = "{\n"
                    "   \"error\": \"Bad Request\",\n"
                    "   \"message\": \"Request body could not be "
                    "read properly\"\n"
                    "}";

    return create_http_response(response, header, "application/json", content,
                                strlen(content) + 1);
} /* http_400() */

/*
 * Creates an HTTP 404 Not Found response, putting it into the response
 * argument.
 */

int http_404(char *response, char *target, file_cont_t **file_cont) {
    fprintf(stderr, "[WARNING] sending HTTP 404 Not Found response\n");

    int status;
    char *header = "HTTP/1.1 404 Not Found";
    *file_cont = read_file_cont(target, &status);

    if (*file_cont == NULL) {
        return SERVER_ERR;
    }

    char *mime = get_mime_type(target);

    return create_http_response(response, header, mime,
                                (*file_cont)->content, (*file_cont)->size);
} /* http_404() */

/*
 * Creates a HTTP 500 Internal Server Error response, putting it into the
 * response argument.
 */

int http_500(char *response) {
    fprintf(stderr, "[WARNING] sending HTTP 500 Internal Server Error "
            "response\n");

    char *header = "HTTP/1.1 500 Internal Server Error";
    char *content = "{\n"
                    "   \"error\": \"Internal Server Error\",\n"
                    "   \"message\": \"Server encountered critical error and "
                    "could not recover\"\n"
                    "}";

    return create_http_response(response, header, "application/json", content,
                                strlen(content) + 1);
} /* http_500() */

/*
 * This function creates an HTTP response, putting it into the given response
 * argument. The HTTP response starts with the given header line, the content's
 * type, some metadata, and then adds the given content. It returns the total
 * size of the HTTP response to be used in send() or SERVER_ERR in case of
 * unrecoverable error.
 */

int create_http_response(char *response, char *header, char *mime,
                         char *content, int size) {
    if (((response == NULL) || (header == NULL)) ||
        ((mime == NULL) || (content == NULL))) {
        return SERVER_ERR;
    }

    int status; 
    int tot_bytes = 0;

    time_t secs = time(NULL);

    if (secs == -1) {
        fprintf(stderr, "[WARNING] unable to get time with time()...leaving"
                        "date blank in HTTP response");
    }

    struct tm *curr_time = NULL;

    if (secs != -1) {
        curr_time = gmtime(&secs);  /* time in GMT */
        if (curr_time == NULL) {
            fprintf(stderr, "[WARNING] unable to transform time with gmtime()"
                            "...leaving date blank in HTTP response");
        }
    }

    char date[DATE_LEN] = { '\0' };
    
    if ((secs != -1) && (curr_time != NULL)) {
        /* formats date like Sun, 16 Aug 2026 14:48:37 GMT */
        /* add 1 because returns number bytes written excluding NUL byte */

        status = strftime(date, sizeof(date), "%a, %d %b %Y %T GMT",
                          curr_time) + 1;
        if (status == 0) {
            fprintf(stderr, "[WARNING] date too long...leaving date blank in"
                    "HTTP response");
        }

        tot_bytes += status;
    }

    status = snprintf(response, RES_LEN,
                      "%s\n"                  /* HTTP status code */
                      "Date: %s\n"            /* formatted date */
                      "Connection: close\n"   /* close TCP connection */
                      "Content-Length: %d\n"  /* not including header */
                      "Content-Type: %s\n\n"  /* mime type */
                      "%s",                   /* content */
                      header, date, size, mime, content) + 1;

    if (status == -1) {
        fprintf(stderr, "[ERROR] unable to create HTTP response with"
                "snprintf()\n");
        return SERVER_ERR;
    }

    if (status >= RES_LEN) {
        fprintf(stderr, "[WARNING] HTTP response too long...truncating"
                "response\n");
        return RES_LEN;
    }

    tot_bytes += status;

    return tot_bytes;
} /* create_http_response() */

/*
 * This function extracts the request's method and target, putting them into
 * the method and target pointer arguments.
 */

void parse_http_request(char *request, char *method, char *target) {
    if ((request == NULL) || ((method == NULL) || (target == NULL)) {
        fprintf(stderr, "[ERROR] unable to parse HTTP request\n");
        return;
    }

    int status;

    char tmp_target[TARGET_LEN] = { '\0' };

    status = sscanf(request, "%19[^ ] %1023[^ ]", method, tmp_target);

    if (status != REQ_VALS_CT) {
        fprintf(stderr, "[ERROR] unable to parse HTTP request\n");
        return;
    }

    /* forms proper relative path to the files the server plans to serve */

    /* solves issue of adding / to ./root ie. prevents ./root/ */
    /* which may result in ./root//index.html for auto serving index.html */

    if (strncmp(tmp_target, "/", TARGET_LEN) == 0) {
        tmp_target[0] = '\0';
    }

    status = snprintf(target, TARGET_LEN, "%s%s", SERVER_FILES, tmp_target);

    if (status >= TARGET_LEN) {
        fprintf(stderr, "[WARNING] requested target is too long...truncating"
                "target\n");
    }
} /* parse_http_request() */

/*
 * Handles the HTTP request from the client by storing it in a buffer, parsing
 * the method and requested target, and formulating an appropriate response.
 */

void handle_http_request(int client_socket) {
    int status;

    char request[REQ_LEN] = { '\0' };

    int recv_bytes = recv(client_socket, request, REQ_LEN - 1, 0);

    if (recv_bytes == -1) {
       fprintf(stderr, "[ERROR] unable to receive HTTP request\n"); 
       return;
    }

    if (recv_bytes == 0) {  /* indicates client closed connection */
        printf("[LOG] client closed connection\n");
        return;
    }

    request[recv_bytes] = '\0'; 

    char method[METHOD_LEN] = { '\0' };  /* HTTP method ie. GET, POST */
    char target[TARGET_LEN] = { '\0' };  /* requested target */

    /* method or target will be empty if malformed request */
    /* checked in handle_http_response */

    parse_http_request(request, method, target);

    handle_http_response(client_socket, method, target);
} /* handle_http_request() */

/*
 * Test
 */

void handle_http_response(int client_socket, char *method, char *target) {
    int status;
    char response[RES_LEN] = { '\0' };

    /* true if parsing request failed which indicates malformed request */

    if ((strlen(method) == 0) || (strlen(target) == 0)) {
        status = check_http_res(response, http_400(response));

        /* indicates unrecoverable server error so should terminate */

        if (status == SERVER_ERR) {
            fprintf(stderr, "[ERROR] terminating in handle_http_response()\n");
            return;
        }

        /* here, status is the size if successfully created response */

        send_response(client_socket, response, status);

        return;
    }

    /* handle attempting to send HTTP 200 response or error response */

    if (strcmp("GET", method) == 0) {
        file_cont_t *file_cont = NULL;

        status = check_http_res(response,
                                http_200(response, target, &file_cont));
        if (status == SERVER_ERR) {
            fprintf(stderr, "[ERROR] terminating in handle_http_response()\n");
            free_file_cont_t(&file_cont);
            return;
        }

        /* here, status size of response */

        send_response(client_socket, response, status);

        free_file_cont_t(&file_cont);

        return;
    } 

    /* handles POST response */

    if (strcmp("POST", method) == 0) {

    } 
} /* handle_http_response() */

/*
 * This function takes in a response and the size of a response and sends it
 * to the client given by the file descriptor. It loops until the full response
 * is sent.
 */

void send_response(int fd, char *response, int size) {
    int status;
    int bytes_sent = 0;

    while (bytes_sent < size) {
        status = send(fd, response, size - bytes_sent, 0);

        if (status == -1) {
            fprintf(stderr, "[ERROR] error while sending response with "
                    "send()\n");
            return;
        }

        bytes_sent += status;
        response += status;
    }
} /* send_response() */
