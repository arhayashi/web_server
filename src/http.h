#ifndef HTTP_H

#define HTTP_H

void handle_http_request(int);
void handle_http_response(int, char *, char *);
int parse_http_request(char *, char *, char *);

#endif
