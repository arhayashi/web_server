#ifndef HTTP_H

#define HTTP_H

int create_http_response(char *, char *, char *, char *, int);
void handle_http_request(int);
void handle_http_response(int, char *, char *, int);
int parse_http_request(char *, char *, char *);

#endif
