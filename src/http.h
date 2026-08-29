#ifndef HTTP_H

#define HTTP_H

#include "process_file.h"

int check_http_res(char *, int);
int create_http_response(char *, char *, char *, char *, int);
void handle_http_request(int);
void handle_http_response(int, char *, char *);
int http_200(char *, char *, file_cont_t **);
int http_400(char *);
int http_404(char *, char *, file_cont_t **);
int http_500(char *);
void parse_http_request(char *, char *, char *);
void send_response(int, char *, int);

#endif
